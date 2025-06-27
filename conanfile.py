import os

from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, CMake, cmake_layout
from conan.tools.files import copy


class UtilityRecipe(ConanFile):
    name = "utility"
    version = "0.3"
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

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.variables["UTILITY_BUILD_SHARED_LIB"] = self.options.shared
        tc.variables["UTILITY_SUPPORT_QT"] = self.options.with_qt
        tc.variables["UTILITY_SUPPORT_BOOST"] = self.options.with_boost
        tc.variables["UTILITY_BUILD_TEST"] = self.options.with_tests
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if self.options.with_tests:
            cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()
