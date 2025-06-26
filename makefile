.PHONY: rebuild build reset clean

rebuild: clean
	@echo "Rebuilding..."
	conan install . --output-folder=build --build=missing --profile=profiles/msvc-142-x86
	cd build && cmake .. -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake"
	cd build && cmake --build . --config Release
	./bin/Release/utility_test.exe

build:
	@echo "Building..."
	cd build && cmake .. -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake"
	cd build && cmake --build . --config Release
	./bin/Release/utility_test.exe

reset:
	@echo "Resetting..."
	rm -fr ./build/CMakeFiles
	rm -fr ./build/CMakeCache.txt

clean:
	@echo "Cleaning..."
	rm -r ./build
