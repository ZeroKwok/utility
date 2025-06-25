.PHONY: build
build:
	@echo "Building..."
	conan install . --output-folder=build --build=missing --profile=profiles/msvc-142-x86
	cd build && cmake .. -DCMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake"
	cd build && cmake --build . --config Release
	./bin/Release/utility_test.exe

reset:
	rm -r ./build/CMakeFiles
	rm -r ./build/CMakeCache.txt

clean:
	@echo "Cleaning..."
	rm -r ./build