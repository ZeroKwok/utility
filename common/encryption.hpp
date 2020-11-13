#ifndef encryption_h__
#define encryption_h__

/*
*   encryption.hpp 
*
*   v0.1 2020-11 by GuoJH
*/

#include <common/bytedata.hpp>
#include <common/common_cfg.hpp>

namespace util {
namespace encryption {

/*!
 *  /brief  ����TEA-32(Tiny Encryption Algorithm)�ķ�ʽ��������.
 *  /param  keyΪ128λ��Կ, С��128λ������0���.
 * 
 *  /note   ���������Ĳ��ȳ�, ���ܻ��׳�std::runtime_error�쳣.
 */
UTILITY_FUNCT_DECL bytedata& encrypt_with_tea32(bytedata& bytes, const bytedata& key);

/*!
 *  /brief  ����TEA-32�ķ�ʽ��������.
 *  /param  bytes ����Ϊ8��������.
 * 
 *  /note   ���ܻ��׳�std::runtime_error�쳣.
 */
UTILITY_FUNCT_DECL bytedata& decrypt_with_tea32(bytedata& bytes, const bytedata& key);

} // encryption
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/encryption.ipp"
#endif

#endif // encryption_h__
