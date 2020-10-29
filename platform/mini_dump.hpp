#ifndef mini_dump_h__
#define mini_dump_h__

/*!
 *  mini_dump.hpp 
 * 
 *  v0.1 2019-02 By GuoJH
 */

#include <functional>
#include <platform/platform_cfg.hpp>
#include <platform/platform_error.hpp>

namespace util{
namespace win {

class mini_dump
{
    mini_dump();
    mini_dump& operator=(mini_dump&);

    friend struct mini_dump_private;
public:
    typedef std::function<void()> closure_type;

    /// ��ö���ʵ��
    static mini_dump& instance();

    /// ������Сת���ļ�
    static void create_dump(const std::wstring file);
    static void create_dump(const std::wstring file, platform_error& error);

    /// ��ʼ������ϵͳע��ṹ���쳣������, �����ڱ���ʱ�õ����ᴴ����Сת���ļ�;
    /// ��Ҫע�����: �ڳ����е���SetUnhandledExceptionFilter()���ø�����ʧЧ;
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
#   include "impl/mini_dump.ipp "
#endif

#endif // mini_dump_h__
