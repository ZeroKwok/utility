mkdir build
cd build
cmake -G "Visual Studio 10 2010" ..
cmake --build . --config debug
cmake --build . --config release
cmake --install . --prefix ./install --config debug
cmake --install . --prefix ./install --config release
xcopy .\install ..\..\..\3rd\utility\ /e