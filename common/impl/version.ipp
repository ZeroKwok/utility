#ifdef UTILITY_DISABLE_HEADONLY
#   include "../version.h"
#endif

#ifndef UTILITY_SUPPORT_BOOST
#   error version.h needs Boost support
#endif

#include <string/string_conv_easy.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace util {

uint32_t version_generate(
    int8_t major, int8_t minor, int8_t build /*= 0*/, int8_t revision /*= 0*/)
{
    return (major << 24) | (minor << 16) | (build << 8) | revision;
}

//!
//!  返回指定版本号的32位值
//!
uint32_t version_into_value(const product_version& version)
{
    return uint32_t(version.major << 24) | uint32_t(version.minor << 16) |
           uint32_t(version.build << 8) | uint32_t(version.revision);
}

//!
//!  返回指定版本号的字符串表示
//!
//!  product_version {0,0,0,0} -> "0.0"
//!  product_version {0,1,0,0} -> "0.1"
//!  product_version {0,1,5,0} -> "0.1.5"
//!  product_version {0,1,0,1} -> "0.1.0.1"
//! 
std::string version_into_string(const product_version& version)
{
    std::string result = util::sformat("%d.%d", version.major, version.minor);

    if (version.revision != 0)
        result += util::sformat(".%d.%d", version.build, version.revision);
    else if (version.build != 0)
        result += util::sformat(".%d", version.build);

    return result;
}

//!
//!  返回指定版本号的宽字符表示
//!
std::wstring version_into_wstring(const product_version& version)
{
    std::wstring result = util::sformat(L"%d.%d", version.major, version.minor);

    if (version.revision != 0)
        result += util::sformat(L".%d.%d", version.build, version.revision);
    else if (version.build != 0)
        result += util::sformat(L".%d", version.build);

    return result;
}

//!
//! 从32位值构造版本号
//! 
product_version version_from_value(uint32_t value)
{
    product_version version = {
        /* 主版本号   */ (value & 0xff000000) >> 24,
        /* 次版本号   */ (value & 0x00ff0000) >> 16,
        /* 构建版本号 */ (value & 0x0000ff00) >> 8,
        /* 修订版本号 */ (value & 0x000000ff)
    };

    return version;
}

//!
//! 从字符串构造版本号
//! 
product_version version_from_string(std::string version)
{
    product_version result = { 0 };

    try
    {
        std::vector<std::string> fields;
        boost::algorithm::split(fields, version, boost::algorithm::is_any_of("."), boost::token_compress_off);

        if (fields.size() >= 1)
            result.major = boost::lexical_cast<uint32_t>(fields[0]);

        if (fields.size() >= 2)
            result.minor = boost::lexical_cast<uint32_t>(fields[1]);

        if (fields.size() >= 3)
            result.build = boost::lexical_cast<uint32_t>(fields[2]);

        if (fields.size() >= 4)
            result.revision = boost::lexical_cast<uint32_t>(fields[3]);
    }
    catch (...)
    {
    }

    return result;
}

product_version version_from_string(std::wstring version)
{
    return version_from_string(conv::easy::_2str(version));
}

} // util