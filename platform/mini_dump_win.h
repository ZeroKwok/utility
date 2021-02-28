#ifndef mini_dump_win_h__
#define mini_dump_win_h__

/*
*  mini_dump_win.h
* 
*  v0.1 2019-02 By GuoJH
*/

#include <functional>
#include <utility.hpp>
#include <platform/platform_cfg.h>
#include <platform/platform_error.h>

#ifndef OS_WIN
#   error mini_dump_win.h only supports Windows.
#endif

namespace util{
namespace win {

class mini_dump
{
    mini_dump();
    mini_dump& operator=(mini_dump&);

    friend struct mini_dump_private;
public:
    typedef std::function<void()> closure_type;

    /// 获得对象实例
    static mini_dump& instance();

    /// 创建最小转储文件
    static void create_dump(const std::wstring file);
    static void create_dump(const std::wstring file, platform_error& error);

    /// 初始化并向系统注册结构化异常过滤器, 用以在崩溃时得到机会创建最小转储文件;
    /// 需要注意的是: 在程序中调用SetUnhandledExceptionFilter()会让该流程失效;
    void init();

    void set_crash_handler(const closure_type& crash_call)
    {
        _crash_handler = crash_call;
    }

protected:
    void crash_hander()
    {
        if(_crash_handler)
            _crash_handler();
    }

protected:
    closure_type _crash_handler;
};

} // win
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/mini_dump_win.ipp "
#endif

#endif // mini_dump_win_h__
