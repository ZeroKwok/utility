import os

from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.files import copy
from conan.tools.microsoft import is_msvc, is_msvc_static_runtime


class UtilityRecipe(ConanFile):
    name = "utility"
    version = "0.3"
    description = "A utility library for C++ 20"

    license = "MIT"
    url = "https://github.com/ZeroKwok/utility"
    homepage = "https://github.com/ZeroKwok/utility"
    author = "Zero <zero.kwok@foxmail.com>"
    topics = ("utility library")

    # package_type should usually be "library", "shared-library" or "static-library"
    package_type = "library"
    settings = "os", "compiler", "build_type", "arch"

    # Define options for the recipe
    options = {
        "shared": [True, False],        # 是否构建动态库
        "with_boost": [True, False],    # 是否启用 Boost
        "with_qt": [True, False],       # 是否启用 Qt
        "with_tests": [True, False]     # 是否构建测试
    }
    default_options = {
        "shared": True,
        "with_qt": False,
        "with_boost": True,
        "with_tests": False
    }

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*", "include/*", "test/*", "cmake/*", "scripts/*"

    def requirements(self):
        if self.options.with_boost:
            self.requires("boost/1.88.0", options={"shared": False})
        if self.options.with_qt:
            self.requires("qt/5.15", options={"shared": True})
        if self.options.with_tests:
            self.requires("gtest/1.16.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.22.6")

    def config_options(self):
        if self.options.shared:
            self.options.rm_safe("compiler.runtime")

    def layout(self):
        self.folders.build_folder_vars = [
            "settings.compiler",
            "settings.compiler.version",
            "settings.arch",
            "options.shared",
            "settings.compiler.runtime",
            "settings.build_type",
        ]
        cmake_layout(self)

        # Define project folder structure
        self.folders.source = "."
        self.folders.build = os.path.join(
            self.folders.source, 
            f"{self.settings.compiler.value}-"
            f"{self.settings.compiler.version.value}-"
            f"{self.settings.arch.value}-"
            f"{'static' if not self.options.shared else 'dynamic'}-"
            f"{self.settings.build_type.value.lower()}")
        self.folders.generators = self.folders.build

    def validate(self):
        # validate the minimum cpp standard supported. For C++ projects only.
        check_min_cppstd(self, 20)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["UTILITY_BUILD_SHARED_LIB"] = 'ON' if self.options.shared else 'OFF'
        tc.variables["UTILITY_SUPPORT_QT"] = 'ON' if self.options.with_qt else 'OFF'
        tc.variables["UTILITY_SUPPORT_BOOST"] = 'ON' if self.options.with_boost else 'OFF'
        tc.variables["UTILITY_BUILD_TEST"] = 'ON' if self.options.with_tests else 'OFF'
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if self.options.with_tests:
            cmake.test()

    def package(self):
        copy(self, "LICENSE", self.source_folder, os.path.join(self.package_folder, "licenses"))
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["utility"]