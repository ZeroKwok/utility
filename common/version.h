#ifndef version_h__
#define version_h__

/*
*   version.h
*
*   v0.1 2021-03 by GuoJH
*   v0.2 2022-11 by GuoJH
*/

#include <string>
#include <common/common_cfg.h>

namespace util {

//!
//!  产品版本号
//!
struct product_version
{
    uint16_t major;      //!< 主版本号
    uint16_t minor;      //!< 次版本号
    uint16_t patch;      //!< 修订版本号
    uint16_t build;      //!< 构建版本号
};

//!
//!  从字段构造版本号
//!
UTILITY_FUNCT_DECL product_version version_make(
    uint16_t major, uint16_t minor, uint16_t patch = 0, uint16_t build = 0);

//!
//! 从整形值构造版本号
//! 
UTILITY_FUNCT_DECL product_version version_from_value(uint64_t value);
UTILITY_FUNCT_DECL product_version version_from_value32(uint32_t value);

//!
//! 从字符串构造版本号
//! 
UTILITY_FUNCT_DECL product_version version_from_string(std::string version);
UTILITY_FUNCT_DECL product_version version_from_string(std::wstring version);

//!
//!  返回指定版本号的整形值
//!
UTILITY_FUNCT_DECL uint64_t version_into_value(const product_version& version);
UTILITY_FUNCT_DECL uint32_t version_into_value32(const product_version& version);

//!
//!  从字符串转换为整形值
//!
UTILITY_FUNCT_DECL uint64_t version_into_value(std::string version);
UTILITY_FUNCT_DECL uint64_t version_into_value(std::wstring version);
UTILITY_FUNCT_DECL uint32_t version_into_value32(std::string version);
UTILITY_FUNCT_DECL uint32_t version_into_value32(std::wstring version);

//!
//!  从字段转换为整形值
//!
UTILITY_FUNCT_DECL uint64_t version_into_value(
    uint16_t major, uint16_t minor, uint16_t patch = 0, uint16_t build = 0);
UTILITY_FUNCT_DECL uint32_t version_into_value32(
    uint16_t major, uint16_t minor, uint16_t patch = 0, uint16_t build = 0);

//!
//!  返回指定版本号的字符串表示
//!  if reserve == 4 then
//!     product_version {0,0,0,0} -> "0.0.0.0"
//!     product_version {0,1,0,0} -> "0.1.0.0"
//!     product_version {0,1,5,0} -> "0.1.5.0"
//!     product_version {0,1,0,1} -> "0.1.0.1"
//!  else if reserve == 3 then
//!     product_version {0,0,0,0} -> "0.0.0"
//!     product_version {0,1,0,0} -> "0.1.0"
//!     product_version {0,1,5,0} -> "0.1.5"
//!     product_version {0,1,0,1} -> "0.1.0.1"
//!  else if reserve == 2 then
//!     product_version {0,0,0,0} -> "0.0"
//!     product_version {0,1,0,0} -> "0.1"
//!     product_version {0,1,5,0} -> "0.1.5"
//!     product_version {0,1,0,1} -> "0.1.0.1"
//!  else if reserve == 1 then
//!     product_version {0,0,0,0} -> "0"
//!     product_version {1,0,0,0} -> "1"
//!     product_version {0,1,0,0} -> "0.1"
//!     product_version {0,1,5,0} -> "0.1.5"
//!     product_version {0,1,0,1} -> "0.1.0.1"
//! 
UTILITY_FUNCT_DECL std::string version_into_string(const product_version& version, int reserve = 3);
UTILITY_FUNCT_DECL std::wstring version_into_wstring(const product_version& version, int reserve = 3);

//!
//!  从整形值转换为字符串表示
//!
UTILITY_FUNCT_DECL std::string version_into_string(uint64_t version, int reserve = 3);
UTILITY_FUNCT_DECL std::wstring version_into_wstring(uint64_t version, int reserve = 3);
UTILITY_FUNCT_DECL std::string version_into_string_from32(uint32_t version, int reserve = 3);
UTILITY_FUNCT_DECL std::wstring version_into_wstring_from32(uint32_t version, int reserve = 3);

//!
//!  从字段转换为字符串表示
//!
UTILITY_FUNCT_DECL std::string version_into_string(
    uint16_t major, uint16_t minor, uint16_t patch = 0, uint16_t build = 0, int reserve = 3);
UTILITY_FUNCT_DECL std::wstring version_into_wstring(
    uint16_t major, uint16_t minor, uint16_t patch = 0, uint16_t build = 0, int reserve = 3);

//!
//!  版本模式匹配(通配符可以指代任何版本号)
//! 
//!  2.* or 2
//!     2.1
//!     2.2.3
//!     2.9.0
//!     2.20.55.86
//!  3.5.* or 3.5
//!     3.5
//!     3.5.0
//!     3.5.1
//!     3.5.86.32
//!  3.*.2
//!     3.0.2
//!     3.2.2
//!  * or *.* or *.*.*
//!     All
//! 
UTILITY_FUNCT_DECL bool version_match(const product_version& version, const std::string& pattern);

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
