.PHONY: rebuild build clean test

PROFILE ?= msvc-142-x86

ifdef DEBUG
	OPTIONS := --settings=build_type=Debug
endif

rebuild: clean
	@echo "Rebuilding ${PROFILE} ..."
	conan install . --output-folder=build --build=missing --profile=profiles/${PROFILE} $(OPTIONS)
	conan build   . --output-folder=build --profile=profiles/${PROFILE} $(OPTIONS)

build:
	@echo "Building ${PROFILE}..."
	conan build   . --output-folder=build --profile=profiles/${PROFILE} $(OPTIONS)

test: clean
	@echo "Running tests..."
	./bin/Release/utility_test.exe

install: clean
	@echo "Installing..."
	conan create . --profile=profiles/${PROFILE} $(OPTIONS)

clean:
	@echo "Cleaning..."
	rm -fr ./build
