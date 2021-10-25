#ifndef acronym_for_pinyin_h__
#define acronym_for_pinyin_h__

/*
*   acronym_for_pinyin.h
*
*   v0.1 2018-12 By GuoJH
*   v0.2 2021-10 by GuoJH
*/

#include <common/common_cfg.h>

namespace util   {

//! @brief 将GBK字符串转换到拼音首字母缩写
//! @param gbk GBK编码的字符串
//! @return 返回拼音首字母缩写
UTILITY_FUNCT_DECL std::string get_acronym(const std::string& gbk);

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/acronym_for_pinyin.ipp"
#endif

#endif // acronym_for_pinyin_h__
