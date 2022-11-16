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
//!
//!  product_version {0,0,0,0} -> "0.0"
//!  product_version {0,1,0,0} -> "0.1"
//!  product_version {0,1,5,0} -> "0.1.5"
//!  product_version {0,1,0,1} -> "0.1.0.1"
//! 
UTILITY_FUNCT_DECL std::string version_into_string(const product_version& version);
UTILITY_FUNCT_DECL std::wstring version_into_wstring(const product_version& version);

//!
//!  从整形值转换为字符串表示
//!
UTILITY_FUNCT_DECL std::string version_into_string(uint64_t version);
UTILITY_FUNCT_DECL std::wstring version_into_wstring(uint64_t version);
UTILITY_FUNCT_DECL std::string version_into_string_from32(uint32_t version);
UTILITY_FUNCT_DECL std::wstring version_into_wstring_from32(uint32_t version);

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
