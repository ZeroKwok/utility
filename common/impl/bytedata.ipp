#ifdef UTILITY_DISABLE_HEADONLY
#   include "../bytedata.hpp"
#endif

#include <vector>
#include <iterator>

namespace util {
namespace detail {

// char内容转为16进制字符
inline bool _hex_from_character(char ch, char& hex)
{
    // is the ch a number(0~9)?
    if (0x30 <= ch && ch <= 0x39)
        hex = ch - 0x30;
    else if (0x41 <= ch && ch <= 0x46) // A~F
        hex = ch - 0x37;
    else if (0x61 <= ch && ch <= 0x66) // a~f
        hex = ch - 0x57;
    else
        return false;

    return true;
}

// base64 支持
#include "base64.hpp"

} // detail

void bytes_into_memory(void* address, int length, const bytedata& bytes)
{
    ::memcpy_s(address, length, &bytes[0], bytes.size());
}

std::string bytes_into_hex(
    const bytedata& bytes, 
    const std::string& separator/* = ""*/, 
    const int percount/* = 1*/)
{
    std::string result;
    const char _hex[] = "0123456789abcdef";

    for (size_t i = 0; i < bytes.size(); ++i)
    {
        result.append(1, _hex[(bytes.at(i) >> 4) & 0x0f]);
        result.append(1, _hex[bytes.at(i) & 0x0f]);

        if ((i + 1) < bytes.size() && i % percount == 0)
            result += separator;
    }

    return result;
}

bytedata bytes_from_hex(const std::string& hex)
{
    std::vector<char> hex_array;

    // 若存在0x, 则跳过
    int pos = 0;
    if (hex.size() > 2)
    {
        if (hex[0] == '0' && hex[1] == 'x')
            pos = 2;
    }

    for (size_t i = pos; i < hex.size(); ++i)
    {
        char h = 0, ch = hex[i];
        if (detail::_hex_from_character(ch, h))
            hex_array.push_back(h);
    }

    bytedata result;
    for (size_t i = 0; i < hex_array.size(); i += 2)
        result.append(1, char(hex_array[i] << 4) | hex_array[i + 1]);

    return result;
}

bytedata& bytes_from_file(
    bytedata& bytes, const util::fpath& name, int offset/* = 0*/, int length/* = -1*/)
{
    util::ffile  file = util::file_open(name, O_RDONLY);
    util::fsize  size = util::file_size(file);

    if (-1 == length)
        length = static_cast<int>(size - offset);

    if (size == 0 && (offset + length) > size)
        throw std::runtime_error("The file too short or param invalid.");

    if (offset != 0)
        util::file_seek(file, offset);

    if (length > 0x3200000) // 50MB
        throw std::runtime_error("A single read cannot be greater than 50MB");

    bytes.resize(length);
    util::file_read(file, &bytes[0], static_cast<int>(bytes.size()));

    return bytes;
}

void bytes_into_file(const util::fpath& name, const bytedata& bytes)
{
    if (!util::file_exist(util::path_find_parent(name)))
        util::directories_create(util::path_find_parent(name));

    util::ffile file = util::file_open(name, O_CREAT | O_TRUNC | O_WRONLY);

    util::file_write(file, bytes.data(), static_cast<int>(bytes.size()));
}

std::string& bytes_into_base64(
    std::string& base64, const bytedata& bytes)
{
    base64.clear();

    std::insert_iterator<std::string> ii(base64, base64.begin());
    detail::base64<char> b64;
    int state = 0;
    b64.get(bytes.begin(), bytes.end(), ii, state);

    return base64;
}

bytedata& bytes_from_base64(
    bytedata& bytes, const std::string& base64, bool noline/* = true*/)
{
    bytes.clear();

    std::insert_iterator<std::string> ii(bytes, bytes.begin());
    detail::base64<char> b64;
    int state = 0;

    if (noline)
    {
        b64.put(base64.begin(), base64.end(), ii, state, detail::base64<>::noline());
    }
    else
    {
#if defined(OS_WIN)
        b64.put(base64.begin(), base64.end(), ii, state, detail::base64<>::crlf());
#else
        b64.put(base64.begin(), base64.end(), ii, state, detail::base64<>::lf());
#endif
    }
    return bytes;
}

} // util