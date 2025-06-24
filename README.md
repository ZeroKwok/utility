# utility

提供一些在日常开发中经常用到，但没有在 C++ 标准库以及 Boost 中提供的功能，或者一些特定于某个平台的扩展功能。

## Requirements

- C++ 20 and leter
- Boost 1.79.0 and leter
- Qt 5.x and leter (Optional)
- CMake 3.15 and leter
- Conan 2

## How to build

```bash
git clone https://github.com/ZeroKwok/utility.git
cd utility
conan install . --output-folder=build --build=missing --profile=profiles/msvc-142-x86
cmake --build build
cmake --install build
```

## How to use

```cpp
#include <utility/utility.hpp>

int main()
{
    return 0;
}
```
