.PHONY: rebuild build clean test

PROFILE ?= msvc-142-x86
OPTIONS := --settings=build_type=$(if $(DEBUG),Debug,Release)

OPTIONS_WITH_OPTIONS := --settings=build_type=Debug --options=utility/*:with_tests=True

rebuild: clean
	@echo "Rebuilding ${PROFILE} ..."
	conan install . --output-folder=build --build=missing --profile=profiles/${PROFILE} $(OPTIONS)
	conan build   . --output-folder=build --profile=profiles/${PROFILE} $(OPTIONS)

build:
	@echo "Building ${PROFILE}..."
	conan build   . --output-folder=build --profile=profiles/${PROFILE} $(OPTIONS)

test: clean
	@echo "Running tests..."
	conan install . --output-folder=build --build=missing --profile=profiles/${PROFILE} $(OPTIONS_WITH_OPTIONS)
	conan build   . --output-folder=build --profile=profiles/${PROFILE} $(OPTIONS_WITH_OPTIONS)

install: clean
	@echo "Installing..."
	conan create . --profile=profiles/${PROFILE} $(OPTIONS)

clean:
	@echo "Cleaning..."
	rm -fr ./build
