#ifndef file_util_h__
#define file_util_h__

/*
*   file_util.hpp 
*   
*   v0.1 2018-3 by GuoJH
*   v0.2 2019-7 by GuoJH
*/

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/file_util.ipp"
#endif

#include <config.h>

namespace util {

/*
*	file system error
*/
class ferror
{
public:
    typedef uint32_t        fsecode_type;
    typedef std::string     fsemsgs_type;

    ferror(fsecode_type code = 0, const fsemsgs_type& desc = fsemsgs_type())
        : _code(code)
        , _desc(desc)
    {}

    void clear()
    {
        _code = 0;
        _desc = fsemsgs_type();
    }

    /*
    *	返回是否发生了错误
    *   这里假设所有平台的错误代码0, 均表示没有发生错误
    */
    operator bool() const
    {
        return _code != 0;
    }

    fsecode_type code() const
    {
        return _code;
    }

    fsemsgs_type error_description() const
    {
        return _desc;
    }

    /*
    *	返回错误代码的详细信息
    *   根据当前平台环境输出错误代码代表的意义
    */
    fsemsgs_type error_message() const; // Different implementations

    fsemsgs_type print() const
    {
        fsemsgs_type msgs = "File System Error: ";
                     msgs += error_description();
                     msgs += util::sformat("\r\nError details: 0x08x, ", code());
                     msgs += error_message();
        return msgs;
    }

private:
    fsecode_type _code;     //系统错误代码
    fsemsgs_type _desc;     //错误描述信息, 不是错误代码的意义
};

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/file_util.ipp"
#endif

#endif // file_util_h__
