#ifndef string_cfg_h__
#define string_cfg_h__

#include <utility.hpp>

#ifdef UTILITY_ENABLE_AUTO_LINK
#   ifndef LINK_STRING
#      define LINK_STRING
#      define AUTO_LINK_LIB_NAME string
#      include <link.hpp>
#   endif
#endif

namespace util {

/*!
 *  \brief 字符编码格式
 */
enum coded_format
{
    format_local = 1,   //!< 本地8位编码格式, 根据平台以及地区不同而不同,
                        //!< 例如: Windows 中国大陆: GB2312, Linux: UTF-8;
    format_utf8  = 2,   //!< UTF-8编码格式
};

} // util

#endif // string_cfg_h__
