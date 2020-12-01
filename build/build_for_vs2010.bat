mkdir build_vs2010
pushd build_vs2010
cmake -G "Visual Studio 10 2010" ../..
cmake --build . --config Debug
cmake --build . --config RelWithDebInfo
cmake --install . --prefix ../utility_vs2010 --config Debug
cmake --install . --prefix ../utility_vs2010 --config RelWithDebInfo

popd
del /S /Q .\build_vs2010
rd /S /Q .\build_vs2010

mkdir build_vs2010
pushd build_vs2010

cmake -G "Visual Studio 10 2010 Win64"../..
cmake --build . --config Debug
cmake --build . --config RelWithDebInfo
cmake --install . --prefix ../utility_vs2010 --config Debug
cmake --install . --prefix ../utility_vs2010 --config RelWithDebInfo

popd
del /S /Q .\build_vs2010
rd /S /Q .\build_vs2010