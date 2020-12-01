mkdir build_vs2019
pushd build_vs2019
cmake -G "Visual Studio 16 2019" -A Win32 ../..
cmake --build . --config Debug
cmake --build . --config RelWithDebInfo
cmake --install . --prefix ../utility_vs2019 --config Debug
cmake --install . --prefix ../utility_vs2019 --config RelWithDebInfo

popd
del /S /Q .\build_vs2019
rd /S /Q .\build_vs2019

mkdir build_vs2019
pushd build_vs2019

cmake -G "Visual Studio 16 2019" -A x64 ../..
cmake --build . --config Debug
cmake --build . --config RelWithDebInfo
cmake --install . --prefix ../utility_vs2019 --config Debug
cmake --install . --prefix ../utility_vs2019 --config RelWithDebInfo

popd
del /S /Q .\build_vs2019
rd /S /Q .\build_vs2019