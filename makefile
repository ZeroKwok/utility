OPTIONS_BASE := --output-folder=build --settings=compiler.cppstd=20
OPTIONS_COMMON := $(OPTIONS_BASE) --settings=build_type=$(if $(DEBUG),Debug,Release)
OPTIONS_WITH_OPTIONS := $(OPTIONS_BASE) --settings=build_type=Debug --options=utility/*:with_tests=True

.PHONY: rebuild build clean test test-build
rebuild: clean
	@echo "Rebuilding ..."
	conan install . --build=missing $(OPTIONS_COMMON)
	conan build   . $(OPTIONS_COMMON)

build:
	@echo "Building ..."
	conan build   . $(OPTIONS_COMMON)

test: clean
	@echo "Running tests..."
	conan install . --build=missing $(OPTIONS_WITH_OPTIONS)
	conan build   . $(OPTIONS_WITH_OPTIONS)

build-test:
	@echo "Building tests ..."
	conan build   . $(OPTIONS_WITH_OPTIONS)

install: clean
	@echo "Installing..."
	conan create . $(OPTIONS_COMMON)

clean:
	@echo "Cleaning..."
	rm -fr ./build
