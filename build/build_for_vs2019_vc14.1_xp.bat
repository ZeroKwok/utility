@echo off
set ProjectName=utility
set ProjectVersion=0.2.1
set ProjectGenerator32Bit="Visual Studio 16 2019" -A Win32 -T v141_xp
set ProjectGenerator64Bit="Visual Studio 16 2019" -A x64 -T v141_xp
set ProjectLibSufix=vc14.1_xp
set ProjectDependPath=G:\local\libraries\boost\boost_1_69_0-msvc-14.1


REM 生成静态运行时32位版本

echo on
mkdir build_vs2019_v141_xp
pushd build_vs2019_v141_xp
cmake -G %ProjectGenerator32Bit% ../.. -DCMAKE_INSTALL_PREFIX:PATH="../%ProjectName%_%ProjectVersion%_static_x86_%ProjectLibSufix%" ^
        -DUTILITY_SUPPORT_QT=OFF  -DUTILITY_BUILD_STATIC_RUNTIME=ON -DUTILITY_SUPPORT_WINXP=ON -DUTILITY_BUILD_TEST=OFF -DUTILITY_SUPPORT_WINXP=ON ^
		-DCMAKE_PREFIX_PATH:STRING="%ProjectDependPath%"
cmake --build   . --config Debug
cmake --build   . --config RelWithDebInfo
cmake --install . --config Debug
cmake --install . --config RelWithDebInfo
popd
del /S /Q .\build_vs2019_v141_xp
rd /S /Q .\build_vs2019_v141_xp


REM 生成动态运行时32位版本

echo on
mkdir build_vs2019_v141_xp
pushd build_vs2019_v141_xp
cmake -G %ProjectGenerator32Bit% ../.. -DCMAKE_INSTALL_PREFIX:PATH="../%ProjectName%_%ProjectVersion%_dynamic_x86_%ProjectLibSufix%" ^
        -DUTILITY_SUPPORT_QT=OFF  -DUTILITY_BUILD_STATIC_RUNTIME=OFF -DUTILITY_SUPPORT_WINXP=ON -DUTILITY_BUILD_TEST=OFF -DUTILITY_SUPPORT_WINXP=ON ^
		-DCMAKE_PREFIX_PATH:STRING="%ProjectDependPath%"
cmake --build   . --config Debug
cmake --build   . --config RelWithDebInfo
cmake --install . --config Debug
cmake --install . --config RelWithDebInfo
popd
del /S /Q .\build_vs2019_v141_xp
rd /S /Q .\build_vs2019_v141_xp

REM REM 生成静态运行时64位版本

REM echo on
REM mkdir build_vs2019_v141_xp
REM pushd build_vs2019_v141_xp
REM cmake -G %ProjectGenerator64Bit% ../.. -DCMAKE_INSTALL_PREFIX:PATH="../%ProjectName%_%ProjectVersion%_static_x64_%ProjectLibSufix%" -DUTILITY_SUPPORT_QT=ON  -DUTILITY_BUILD_STATIC_RUNTIME=ON -DUTILITY_BUILD_TEST=OFF -DCMAKE_PREFIX_PATH:STRING="%ProjectDependPath%"
REM cmake --build   . --config Debug
REM cmake --build   . --config RelWithDebInfo
REM cmake --install . --config Debug
REM cmake --install . --config RelWithDebInfo
REM popd
REM del /S /Q .\build_vs2019_v141_xp
REM rd /S /Q .\build_vs2019_v141_xp


REM REM 生成动态运行时64位版本

REM echo on
REM mkdir build_vs2019_v141_xp
REM pushd build_vs2019_v141_xp
REM cmake -G %ProjectGenerator64Bit% ../.. -DCMAKE_INSTALL_PREFIX:PATH="../%ProjectName%_%ProjectVersion%_dynamic_x64_%ProjectLibSufix%" -DUTILITY_SUPPORT_QT=ON  -DUTILITY_BUILD_STATIC_RUNTIME=OFF -DUTILITY_BUILD_TEST=OFF -DCMAKE_PREFIX_PATH:STRING="%ProjectDependPath%"
REM cmake --build   . --config Debug
REM cmake --build   . --config RelWithDebInfo
REM cmake --install . --config Debug
REM cmake --install . --config RelWithDebInfo
REM popd
REM del /S /Q .\build_vs2019_v141_xp
REM rd /S /Q .\build_vs2019_v141_xp


