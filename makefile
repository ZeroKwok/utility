.PHONY: rebuild build clean test

rebuild: clean
	@echo "Rebuilding..."
	conan install . --output-folder=build --build=missing --profile=profiles/msvc-142-x86
	conan build   . --output-folder=build --profile=profiles/msvc-142-x86

build:
	@echo "Building..."
	conan build   . --output-folder=build --profile=profiles/msvc-142-x86

test: clean
	@echo "Running tests..."
	./bin/Release/utility_test.exe

clean:
	@echo "Cleaning..."
	rm -folderr ./build
