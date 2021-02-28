#ifndef encryption_h__
#define encryption_h__

/*
*   encryption.h 
*
*   v0.1 2020-11 by GuoJH
*/

#include <common/bytedata.hpp>
#include <common/common_cfg.h>

namespace util {
namespace encryption {

/*!
 *  /brief  采用TEA-32(Tiny Encryption Algorithm)的方式加密数据.
 *  /param  key为128位密钥, 小于128位将会以0填充.
 * 
 *  /note   密文与明文不等长, 可能会抛出std::runtime_error异常.
 */
UTILITY_FUNCT_DECL bytedata& encrypt_with_tea32(bytedata& bytes, const bytedata& key);

/*!
 *  /brief  采用TEA-32的方式解密数据.
 *  /param  bytes 必须为8的整倍数.
 * 
 *  /note   可能会抛出std::runtime_error异常.
 */
UTILITY_FUNCT_DECL bytedata& decrypt_with_tea32(bytedata& bytes, const bytedata& key);

} // encryption
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/encryption.ipp"
#endif

#endif // encryption_h__
