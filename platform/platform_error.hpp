#ifndef platform_error_h__
#define platform_error_h__

/*
*   platform_error.hpp 
*
*   v0.1 2020-10 by GuoJH
*/

#include <string>
#include <platform/platform_cfg.hpp>

namespace util {

/*!
*   提供一个与平台相关的错误
*/
class platform_error
{
public:
    typedef int             code_type;
    typedef std::string     msgs_type;

    platform_error(code_type code = 0, const msgs_type& desc = msgs_type())
        : _code(code)
        , _desc(desc)
    {}

    void clear()
    {
        _code = 0;
        _desc = msgs_type();
    }

    /*
    *	返回是否发生了错误
    *   这里假设所有平台的错误代码0, 均表示没有发生错误
    */
    operator bool() const
    {
        return _code != 0;
    }

    code_type code() const
    {
        return _code;
    }

    msgs_type error_description() const
    {
        return _desc;
    }

    /*
    *	返回错误代码的详细信息
    *   根据当前平台环境输出错误代码代表的意义
    */
    msgs_type error_message() const; // Different implementations

    msgs_type print() const;

private:
    code_type _code;     //系统错误代码
    msgs_type _desc;     //错误描述信息, 不是错误代码的意义
};

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/platform_error.ipp"
#endif

#endif // platform_error_h__
