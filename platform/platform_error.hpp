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
*   �ṩһ����ƽ̨��صĴ���
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
    *	�����Ƿ����˴���
    *   �����������ƽ̨�Ĵ������0, ����ʾû�з�������
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
    *	���ش���������ϸ��Ϣ
    *   ���ݵ�ǰƽ̨����������������������
    */
    msgs_type error_message() const; // Different implementations

    msgs_type print() const;

private:
    code_type _code;     //ϵͳ�������
    msgs_type _desc;     //����������Ϣ, ���Ǵ�����������
};

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/platform_error.ipp"
#endif

#endif // platform_error_h__
