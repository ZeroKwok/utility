# utility

Provides features that are commonly used in daily development but not in the C++ standard library or Boost, or platform-specific extensions.

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
make rebuild && make test
make install
```

Configuration and compile types can be specified at build time, for example:

`make rebuild PROFILE=msvc-142-x86-mt DEBUG=1`

## How to use

```cmake
find_package(utility REQUIRED)
target_link_libraries(${PROJECT_NAME} utility)
```

```cpp
#include <utility/utility.h>

int main()
{
    return 0;
}
```
