#ifndef digest_h__
#define digest_h__

/*
*   digest.hpp
*
*   v0.1 2018-09 by GuoJH
*/

#ifdef UTILITY_SUPPORT_BOOST
#   include <boost/uuid/detail/md5.hpp>
#   include <boost/uuid/detail/sha1.hpp>
#endif

#include <common/bytedata.hpp>
#include <common/common_cfg.h>

namespace util {

// need boost support 
#ifdef UTILITY_SUPPORT_BOOST

/// 计算数据的sha1摘要
inline bytedata bytes_sha1_digest(const bytedata& bytes)
{
    boost::uuids::detail::sha1 sha1;
    boost::uuids::detail::sha1::digest_type digest = { 0 };

    sha1.process_bytes(bytes.data(), bytes.size());
    sha1.get_digest(digest);

    bytedata result;
    for (unsigned int i = 0; i < arraysize(digest); i++)
        result += bytes_from(hton_type(digest[i]));

    return result;
}

/// 计算文件的sha1摘要
inline bytedata file_sha1_digest(const fpath& name)
{
    boost::uuids::detail::sha1 sha1;
    boost::uuids::detail::sha1::digest_type digest = { 0 };

    ffile file = util::file_open(name, O_RDONLY);
    fsize size = util::file_size(file);

    uint64_t len = 0;
    do
    {
        bytedata buf(std::min<size_t>(1024 * 512, size_t(size - len)), 0);
        util::file_read(file, &buf[0], buf.size());

        sha1.process_bytes(buf.data(), buf.size());

        len += buf.length();
    } 
    while (len < size);

    sha1.get_digest(digest);

    bytedata result;
    for (unsigned i = 0; i < arraysize(digest); i++)
        result += bytes_from(hton_type(digest[i]));

    return result;
}

/// 计算数据的md5摘要
inline bytedata bytes_md5_digest(const bytedata& bytes)
{
    boost::uuids::detail::md5 md5;
    boost::uuids::detail::md5::digest_type digest;

    md5.process_bytes(bytes.data(), bytes.size());
    md5.get_digest(digest);

    bytedata result;
    for (unsigned i = 0; i < arraysize(digest); i++)
        result += bytes_from(digest[i]);

    return result;
}

/// 计算文件的md5摘要
inline bytedata file_md5_digest(const fpath& name)
{
    boost::uuids::detail::md5 md5;
    boost::uuids::detail::md5::digest_type digest;

    ffile file = util::file_open(name, O_RDONLY);
    fsize size = util::file_size(file);

    uint64_t len = 0;
    do
    {
        bytedata buf(std::min<size_t>(1024 * 512, size_t(size - len)), 0);
        util::file_read(file, &buf[0], buf.size());

        md5.process_bytes(buf.data(), buf.size());

        len += buf.length();
    } 
    while (len < size);

    md5.get_digest(digest);

    bytedata result;
    for (unsigned i = 0; i < arraysize(digest); i++)
        result += bytes_from(digest[i]);

    return result;
}

#endif // UTILITY_SUPPORT_BOOST

} // util


#endif // digest_h__