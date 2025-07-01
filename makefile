.PHONY: rebuild build clean test

PROFILE ?= msvc-142-x86

rebuild: clean
	@echo "Rebuilding ${PROFILE} ..."
	conan install . --output-folder=build --build=missing --profile=profiles/${PROFILE}
	conan build   . --output-folder=build --profile=profiles/${PROFILE}

build:
	@echo "Building ${PROFILE}..."
	conan build   . --output-folder=build --profile=profiles/${PROFILE}

test: clean
	@echo "Running tests..."
	./bin/Release/utility_test.exe

clean:
	@echo "Cleaning..."
	rm -fr ./build
