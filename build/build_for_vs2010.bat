@echo off
set ProjectName=utility
set ProjectVersion=0.2.1
set ProjectGenerator32Bit="Visual Studio 10 2010" -A Win32
set ProjectGenerator64Bit="Visual Studio 10 2010" -A x64
set ProjectLibSufix=vc10.0
set ProjectDependPath=G:\local\libraries\boost\boost_1_66_0_msvc_10
set BoostIncDir=G:/local/libraries/boost/boost_1_66_0_msvc_10
set BoostLibDir=G:/local/libraries/boost/boost_1_66_0_msvc_10/lib32-msvc-10.0

REM 生成静态运行时32位版本

echo on
mkdir build_vs2010
pushd build_vs2010
cmake -G %ProjectGenerator32Bit% ../.. -DCMAKE_INSTALL_PREFIX:PATH="../%ProjectName%_%ProjectVersion%_static_x86_%ProjectLibSufix%" ^
        -DUTILITY_SUPPORT_QT=OFF  -DUTILITY_ENABLE_STATIC_RUNTIME=ON -DUTILITY_BUILD_TEST=OFF -DCMAKE_PREFIX_PATH:STRING="%ProjectDependPath%" ^
		-DBoost_INCLUDE_DIRS=%BoostIncDir% -DBoost_LIBRARY_DIRS=%BoostLibDir% -DUTILITY_SUPPORT_WINXP=ON
cmake --build   . --config Debug
cmake --build   . --config RelWithDebInfo
cmake --install . --config Debug
cmake --install . --config RelWithDebInfo
popd
del /S /Q .\build_vs2010
rd /S /Q .\build_vs2010


REM 生成动态运行时32位版本

echo on
mkdir build_vs2010
pushd build_vs2010
cmake -G %ProjectGenerator32Bit% ../.. -DCMAKE_INSTALL_PREFIX:PATH="../%ProjectName%_%ProjectVersion%_dynamic_x86_%ProjectLibSufix%" ^
        -DUTILITY_SUPPORT_QT=OFF  -DUTILITY_ENABLE_STATIC_RUNTIME=OFF -DUTILITY_BUILD_TEST=OFF -DCMAKE_PREFIX_PATH:STRING="%ProjectDependPath%" ^
		-DBoost_INCLUDE_DIRS=%BoostIncDir% -DBoost_LIBRARY_DIRS=%BoostLibDir%
cmake --build   . --config Debug
cmake --build   . --config RelWithDebInfo
cmake --install . --config Debug
cmake --install . --config RelWithDebInfo
popd
del /S /Q .\build_vs2010
rd /S /Q .\build_vs2010

REM REM 生成静态运行时64位版本

REM echo on
REM mkdir build_vs2010
REM pushd build_vs2010
REM cmake -G %ProjectGenerator64Bit% ../.. -DCMAKE_INSTALL_PREFIX:PATH="../%ProjectName%_%ProjectVersion%_static_x64_%ProjectLibSufix%" -DUTILITY_SUPPORT_QT=ON  -DUTILITY_ENABLE_STATIC_RUNTIME=ON -DUTILITY_BUILD_TEST=OFF -DCMAKE_PREFIX_PATH:STRING="%ProjectDependPath%"
REM cmake --build   . --config Debug
REM cmake --build   . --config RelWithDebInfo
REM cmake --install . --config Debug
REM cmake --install . --config RelWithDebInfo
REM popd
REM del /S /Q .\build_vs2010
REM rd /S /Q .\build_vs2010


REM REM 生成动态运行时64位版本

REM echo on
REM mkdir build_vs2010
REM pushd build_vs2010
REM cmake -G %ProjectGenerator64Bit% ../.. -DCMAKE_INSTALL_PREFIX:PATH="../%ProjectName%_%ProjectVersion%_dynamic_x64_%ProjectLibSufix%" -DUTILITY_SUPPORT_QT=ON  -DUTILITY_ENABLE_STATIC_RUNTIME=OFF -DUTILITY_BUILD_TEST=OFF -DCMAKE_PREFIX_PATH:STRING="%ProjectDependPath%"
REM cmake --build   . --config Debug
REM cmake --build   . --config RelWithDebInfo
REM cmake --install . --config Debug
REM cmake --install . --config RelWithDebInfo
REM popd
REM del /S /Q .\build_vs2010 
REM rd /S /Q .\build_vs2010

