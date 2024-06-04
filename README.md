# utility

该库提供一些在日常开发中经常用到，但没有在 C++ 标准库以及 Boost 中提供的功能，或者一些特定于某个平台的扩展功能。

## Requirements

- C++17 and leter
- Boost 1.79.0 and leter
- CMake 3.13 and leter
- Python 3.8 and leter
- Qt 5.x and leter (Optional)

## How to build

```bash
git clone https://github.com/xxxx/utility.git
cd utility && mkdir build && cd build
cmake -G "Visual Studio 16 2019" .. -DCMAKE_TOOLCHAIN_FILE="vcpkg\scripts\buildsystems\vcpkg.cmake" -DCMAKE_INSTALL_PREFIX="../installed" 
cmake --build .
cmake --install .
```

## How to use

```cpp
#include <utility/utility.hpp>

int main()
{
    util::();
    return 0;
}
```
