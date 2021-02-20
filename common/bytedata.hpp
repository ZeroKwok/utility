#ifndef bytedata_h__
#define bytedata_h__

/*
*   bytedata.hpp 
*
*   v0.1 2018-05 by GuoJH
*   v0.2 2023-10 by GuoJH
*/

#include <string>
#include <common/common_cfg.hpp>
#include <filesystem/path_util.hpp>

#ifdef UTILITY_SUPPORT_BOOST
#   include <boost/archive/binary_oarchive.hpp>
#   include <boost/archive/binary_iarchive.hpp>
#   include <boost/serialization/serialization.hpp>
#endif

#ifdef UTILITY_SUPPORT_QT
#   include <QByteArray>
#endif

namespace util {

/*!
 *  /brief  字节数据, 通过std::string作为底层实现;
 */
class bytedata : public std::string
{
public:
    typedef std::string supper_type;

    bytedata() {}
    bytedata(const char* buf)
        : supper_type(buf) {}
    bytedata(size_t count, char ch)
        : supper_type(count, ch) {}
    bytedata(const supper_type& other)
        : supper_type(other) {}
    bytedata(const void* buf, size_t lenght)
        : supper_type(reinterpret_cast<const char*>(buf), lenght) 
    {}

    const supper_type& string() { 
        return *this; 
    }
 
    void swap(bytedata& other) { 
        supper_type::swap(other); 
    }
    
    void swap(supper_type& other) { 
        supper_type::swap(other); 
    }
};

/*!
 *   Calculates the size of memory used by the content between the start and end.
 */
template<class _Left, class _Right>
inline int size_of_rang(const _Left* start, const _Right* end)
{
    return int(end) + sizeof(_Right) - int(start);
}

/*!
 *   Serialize the content that between the start and end into byte-data.
 */
template<class _Left, class _Right>
inline bytedata bytes_from_rang(const _Left* start, const _Right* end)
{
    return bytedata((const char*)start, size_of_rang(start, end));
}

/*!
 *   Converts any POD(Plain Old Data) type to byte data.
 */
template<class _Type>
inline bytedata bytes_from(const _Type& value)
{
    return bytedata(reinterpret_cast<const char*>(&value), sizeof(_Type));
}

/*!
 *   CONFIG SUPPORT QT
 */
#ifdef UTILITY_SUPPORT_QT
template<>
inline bytedata bytes_from<QByteArray>(const QByteArray& value)
{
    return bytedata(value.data(), value.size());
}
#endif

/*!
 *   Returns byte data for the specified range through the specified address offset.
 */
inline bytedata bytes_range(
    const bytedata& bytes, size_t offset, size_t length)
{
    if (bytes.empty())
        throw std::runtime_error("Original data is empty.");

    if (offset > bytes.size() || bytes.size() < length || (offset + length) > bytes.size())
        throw std::runtime_error("Specified range error.");

#pragma warning( push )
#pragma warning( disable : 4172 )
    return bytes.substr(offset, length);
#pragma warning( pop )
}

/*!
 *   Converts byte data to any POD(Plain Old Data) type.
 */
template<class _Type>
inline _Type bytes_cast(const bytedata& bytes)
{
    if (bytes.size() < sizeof(_Type))
        throw std::runtime_error("The data is too short.");

    return *reinterpret_cast<const _Type*>(&bytes[0]);
}

/*!
 *   Converts byte data to any POD(Plain Old Data) type by specified offset.
 */
template<class _Type>
inline _Type bytes_cast(const bytedata& bytes, size_t offset)
{
    return bytes_cast<_Type>(bytes_range(bytes, offset, sizeof(_Type)));
}

/*!
 *  Converts the byte data into the container.
 */
template<class _Type>
inline _Type bytes_into_container(const bytedata& bytes)
{
    return _Type(bytes.begin(), bytes.end());
}

/*!
 *  From the host byte-order transform into the network byte-order.
 */
template<class _Type>
inline _Type hton_type(_Type value)
{
#ifdef CPU_LITTLE_ENDIAN
    unsigned char bytes[sizeof(_Type)] = { 0 };

    for (unsigned n = 0; n < sizeof(_Type); ++n)
    {
        bytes[sizeof(_Type) - n - 1] =
            static_cast<unsigned char>((value >> (n * 8)) & 0xff);
    }
    return *reinterpret_cast<_Type*>(bytes);

#else
    return value;
#endif
}

/*!
 *  From the network byte-order transform into the host byte-order.
 */
template<class _Type>
inline _Type ntoh_type(_Type value)
{
    return hton_type(value); // Same implementation
}

#ifdef UTILITY_SUPPORT_BOOST

/*!
 *  Serialize the object into byte data.
 *  throw: boost::archive::archive_exception.
 */
template<class _Type>
inline bytedata& bytes_serialize(bytedata& bytesRef, const _Type& type)
{
    std::ostringstream os;
    boost::archive::binary_oarchive oa(os);
    oa << type;

    return bytesRef = os.str();
}
    
/*!
 *  Deserialize from the byte data to objects.
 *  throw: boost::archive::archive_exception.
 */
template<class _Type>
inline _Type& bytes_deserialize(_Type& typeRef, const bytedata& bytes)
{
    std::istringstream is(bytes);
    boost::archive::binary_iarchive ia(is);
    ia >> typeRef;
    
    return typeRef;
}
    
#endif

/*!
 *  Copy the byte data into the memory.
 */
UTILITY_FUNCT_DECL void bytes_into_memory(void* address, int length, const bytedata& bytes);

/*!
 *   Format byte data as a string in hexadecimal.
 */
UTILITY_FUNCT_DECL std::string bytes_into_hex(
    const bytedata& bytes, const std::string& separator = "", const int percount = 1);

/*!
 *   Converts hexadecimal string to byte data.
 */
UTILITY_FUNCT_DECL bytedata bytes_from_hex(const std::string& hex);

/*!
 *  /brief  Read bytes from the file.
 *
 *  /param name     目标文件名称
 *  /param bytes    输出字节数据
 *  /param offset   读取偏移地址
 *  /param length   读取数据长度
 */
UTILITY_FUNCT_DECL bytedata& bytes_from_file(
    bytedata& bytes, const util::fpath& name, int offset = 0, int length = -1);

/*!
 *  /brief  Writes byte data into the file.
 * 
 *  /note   If the directory does not exist, try creating it.
 *          The util::ferror exception is thrown when an error occurs.
 */
UTILITY_FUNCT_DECL void bytes_into_file(const util::fpath& name, const bytedata& bytes);

/*!
 *   Converts byte data to base64 string.
 */
UTILITY_FUNCT_DECL std::string& bytes_into_base64(
    std::string& base64, const bytedata& bytes);

/*!
 *   Converts base64 string to byte data.
 */
UTILITY_FUNCT_DECL bytedata& bytes_from_base64(
    bytedata& bytes, const std::string& base64, bool noline = true);

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/bytedata.ipp"
#endif

#endif // bytedata_h__
