#ifndef version_h__
#define version_h__

#include <string>
#include <common/common_cfg.h>

namespace util {

//!
//!  产品版本号
//!
struct product_version
{
    int8_t major;      //!< 主版本号
    int8_t minor;      //!< 次版本号
    int8_t build;      //!< 构建版本号
    int8_t revision;   //!< 修订版本号
};

//!
//!  返回生产版本号的32位值
//!
UTILITY_FUNCT_DECL uint32_t version_generate(
    int8_t major, int8_t minor, int8_t build = 0, int8_t revision = 0);

//!
//!  返回指定版本号的32位值
//!
UTILITY_FUNCT_DECL uint32_t version_into_value(const product_version& version);

//!
//!  返回指定版本号的字符串表示
//!
//!  product_version {0,0,0,0} -> "0.0"
//!  product_version {0,1,0,0} -> "0.1"
//!  product_version {0,1,5,0} -> "0.1.5"
//!  product_version {0,1,0,1} -> "0.1.0.1"
//! 
UTILITY_FUNCT_DECL std::string version_into_string(const product_version& version);

//!
//!  返回指定版本号的宽字符表示
//!
UTILITY_FUNCT_DECL std::wstring version_into_wstring(const product_version& version);

//!
//! 从32位值构造版本号
//! 
UTILITY_FUNCT_DECL product_version version_from_value(uint32_t value);

//!
//! 从字符串构造版本号
//! 
UTILITY_FUNCT_DECL product_version version_from_string(std::string version);
UTILITY_FUNCT_DECL product_version version_from_string(std::wstring version);

//!
//!  版本类型比较
//!
inline bool operator>(const product_version& left, const product_version& right)
{
    return version_into_value(left) > version_into_value(right);
}

inline bool operator<(const product_version& left, const product_version& right)
{
    return version_into_value(left) < version_into_value(right);
}

inline bool operator==(const product_version& left, const product_version& right)
{
    return version_into_value(left) == version_into_value(right);
}

inline bool operator!=(const product_version& left, const product_version& right)
{
    return version_into_value(left) != version_into_value(right);
}

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/version.ipp"
#endif

#endif // version_h__
