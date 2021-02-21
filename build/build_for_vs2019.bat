mkdir build_vs2019
pushd build_vs2019
cmake -G "Visual Studio 16 2019" -A Win32 ../.. -DCMAKE_INSTALL_PREFIX="../utility_0.2.1_static_x86_vc14.2" -DUTILITY_ENABLE_STATIC_RUNTIME=ON -DUTILITY_BUILD_TEST=OFF -DCMAKE_PREFIX_PATH="G:\local\libraries\boost\boost_1_69_0-msvc-14.1"
cmake --build . --config Debug
cmake --build . --config RelWithDebInfo
cmake --install . --config Debug
cmake --install . --config RelWithDebInfo
popd
del /S /Q .\build_vs2019
rd /S /Q .\build_vs2019

mkdir build_vs2019
pushd build_vs2019
cmake -G "Visual Studio 16 2019" -A Win32 ../.. -DCMAKE_INSTALL_PREFIX="../utility_0.2.1_dynamic_x86_vc14.2" -DUTILITY_ENABLE_STATIC_RUNTIME=OFF -DUTILITY_BUILD_TEST=OFF -DCMAKE_PREFIX_PATH="G:\local\libraries\boost\boost_1_69_0-msvc-14.1"
cmake --build . --config Debug
cmake --build . --config RelWithDebInfo
cmake --install . --config Debug
cmake --install . --config RelWithDebInfo
popd
del /S /Q .\build_vs2019
rd /S /Q .\build_vs2019

mkdir build_vs2019
pushd build_vs2019
cmake -G "Visual Studio 16 2019" -A x64 ../.. -DCMAKE_INSTALL_PREFIX="../utility_0.2.1_static_x64_vc14.2" -DUTILITY_ENABLE_STATIC_RUNTIME=ON -DUTILITY_BUILD_TEST=OFF -DCMAKE_PREFIX_PATH="G:\local\libraries\boost\boost_1_69_0-msvc-14.1"
cmake --build . --config Debug
cmake --build . --config RelWithDebInfo
cmake --install . --config Debug
cmake --install . --config RelWithDebInfo
popd
del /S /Q .\build_vs2019
rd /S /Q .\build_vs2019

mkdir build_vs2019
pushd build_vs2019
cmake -G "Visual Studio 16 2019" -A x64 ../.. -DCMAKE_INSTALL_PREFIX="../utility_0.2.1_dynamic_x64_vc14.2" -DUTILITY_ENABLE_STATIC_RUNTIME=OFF -DUTILITY_BUILD_TEST=OFF -DCMAKE_PREFIX_PATH="G:\local\libraries\boost\boost_1_69_0-msvc-14.1"
cmake --build . --config Debug
cmake --build . --config RelWithDebInfo
cmake --install . --config Debug
cmake --install . --config RelWithDebInfo
popd
del /S /Q .\build_vs2019
rd /S /Q .\build_vs2019

