/*
*   string_conv_iconv.hpp
*
*   v0.1  2018-04 By GuoJH
*   v0.2  2020-08 By GuoJH
*/

#ifndef string_conv_iconv_h__
#define string_conv_iconv_h__

#include <errno.h>
#include <iconv.h>
#include <string.h>
#include <string>
#include <stdexcept>

namespace util {
namespace conv {

template<class InString, class OutString>
OutString& convert_with_iconv(
    const    InString& input,
            OutString& output,
    const std::string& in_encode,
    const std::string& out_encode,
    const         bool ignore_error = false)
{
    // 1. both GLIBC and libgnuiconv will use the locale's encoding 
    //    if ininbuf or outbuf is an empty string.
    // 2. In case of error, it sets errno and returns (iconv_t) -1.
    iconv_t conv = ::iconv_open(out_encode.c_str(), in_encode.c_str());

    if (conv == (iconv_t)-1)
    {
        if (errno == EINVAL)
        {
            throw std::runtime_error(
                "not supported from " + in_encode + " to " + out_encode);
        }
        else
        {
            int error = errno;
            throw std::runtime_error(
                "iconv_open() failed: " + std::to_string(error) + ", " + strerror(error));
        }
    }

    struct guard
    {
        iconv_t _conv;

        guard(iconv_t conv)
            : _conv(conv) {}
        ~guard() { ::iconv_close(_conv); }
    }
    _guard(conv);

    // copy the string to a buffer as iconv function requires a non-const char pointer.
    std::string in_buf(
        reinterpret_cast<const char*>(&input[0]), 
        input.size() * sizeof(typename InString::value_type));

    char * src_ptr  = &in_buf[0];
    size_t src_size = in_buf.size();

    std::string       buf(1024, 0);
    std::string       dst;

    while (0 < src_size)
    {
        char * dst_ptr  = &buf[0];
        size_t dst_size = buf.size();

        size_t res = ::iconv(conv, &src_ptr, &src_size, &dst_ptr, &dst_size);

        if (res == (size_t)-1)
        {
            if (errno == E2BIG) // ���������û�и���Ŀռ�������һ��ת���ַ�
            {
                // ignore this error
            }
            else if (ignore_error)
            {
                // skip character
                ++src_ptr;
                --src_size;
            }
            else
            {
                switch (errno)
                {
                case EILSEQ:  // ����������������Ч�Ķ��ֽ�����, *inbufָ����Ч�Ķ��ֽ����еĿ�ͷ.
                case EINVAL:  // �������������˲������Ķ��ֽ�����, *inbufָ���������ֽ����еĿ�ͷ.
                    throw std::runtime_error("invalid or incomplete multibyte or wide character");

                default:
                {
                    int error = errno;
                    throw std::runtime_error(
                        "iconv() failed: " + std::to_string(error) + ", " + strerror(error));
                }
                }
            }
        }
        dst.append(&buf[0], buf.size() - dst_size);
    }

    output.assign(
        reinterpret_cast<typename OutString::value_type*>(&dst[0]),
        dst.size() / sizeof(typename OutString::value_type));

    return output;
}

} // conv
} // util

#endif // string_conv_iconv_h__
