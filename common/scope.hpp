#ifndef scope_h__
#define scope_h__

#include <string>
#include <functional>
#include <common/common_cfg.h>

namespace util {

class scope_exit
{
    std::function<void()> _closure;
public:

    scope_exit(const std::function<void()>& closure)
        : _closure(closure)
    {}

    ~scope_exit()
    {
        if (_closure)
            _closure();
    }

    scope_exit(const scope_exit&) = delete;
    scope_exit& operator=(const scope_exit&) = delete;
};

} // util

#define util_scope_exit util::scope_exit _scope_exit

#endif // scope_h__
