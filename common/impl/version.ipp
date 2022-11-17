#ifdef UTILITY_DISABLE_HEADONLY
#   include "../version.h"
#endif

#ifndef UTILITY_SUPPORT_BOOST
#   error version.h needs Boost support
#endif

#include <string/string_conv_easy.hpp>
#include <platform/platform_util.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace util {

product_version version_make(
    uint16_t major, uint16_t minor, uint16_t patch /*= 0*/, uint16_t build /*= 0*/)
{
    return { major, minor, patch, build };
}

product_version version_from_value(uint64_t value)
{
    product_version version = {
        /* major */ uint16_t((value >> 48) & 0xffff),
        /* minor */ uint16_t((value >> 32) & 0xffff),
        /* patch */ uint16_t((value >> 16) & 0xffff),
        /* build */ uint16_t(value & 0xffff),
    };

    return version;
}

product_version version_from_value32(uint32_t value)
{
    product_version version = {
        /* major */ (value & 0xff000000) >> 24,
        /* minor */ (value & 0x00ff0000) >> 16,
        /* patch */ (value & 0x0000ff00) >> 8,
        /* build */ (value & 0x000000ff)
    };

    return version;
}

product_version version_from_string(std::string version)
{
    product_version result = { 0 };

    try
    {
        std::vector<std::string> fields;
        boost::algorithm::split(fields, version, boost::algorithm::is_any_of("."), boost::token_compress_off);

        if (fields.size() >= 1)
            result.major = boost::lexical_cast<uint32_t>(fields[0]);

        if (fields.size() >= 2)
            result.minor = boost::lexical_cast<uint32_t>(fields[1]);

        if (fields.size() >= 3)
            result.patch = boost::lexical_cast<uint32_t>(fields[2]);

        if (fields.size() >= 4)
            result.build = boost::lexical_cast<uint32_t>(fields[3]);
    }
    catch (const std::exception& e)
    {
        util::output_debug_string("version_from_string() exception: " + std::string(e.what()));
    }

    return result;
}

product_version version_from_string(std::wstring version)
{
    return version_from_string(conv::easy::_2str(version));
}

uint64_t version_into_value(const product_version& version)
{
    return version_into_value(version.major, version.minor, version.patch, version.build);
}

uint32_t version_into_value32(const product_version& version)
{
    return version_into_value32(version.major, version.minor, version.patch, version.build);
}

uint64_t version_into_value(
    uint16_t major, uint16_t minor, uint16_t patch /*= 0*/, uint16_t build /*= 0*/)
{
    return (uint64_t(major) << 48) | (uint64_t(minor) << 32) | (uint64_t(patch) << 16) | build;
}

uint32_t version_into_value32(
    uint16_t major, uint16_t minor, uint16_t patch /*= 0*/, uint16_t build /*= 0*/)
{
    if (major & 0xff00 ||
        minor & 0xff00 ||
        patch & 0xff00 ||
        build & 0xff00)
    {
        util::output_debug_string(L"version_generate32() warning: High version data will be lost.");
    }

    return ((0x00ff & major) << 24) | ((0x00ff & minor) << 16) | ((0x00ff & patch) << 8) | (0x00ff & build);
}

uint64_t version_into_value(std::string version)
{
    return version_into_value(version_from_string(version));
}

uint64_t version_into_value(std::wstring version)
{
    return version_into_value(version_from_string(version));
}

uint32_t version_into_value32(std::string version)
{
    return version_into_value32(version_from_string(version));
}

uint32_t version_into_value32(std::wstring version)
{
    return version_into_value32(version_from_string(version));
}

std::string version_into_string(const product_version& version, int reserve /*= 3*/)
{
    reserve = std::max(std::min(4, reserve), 1);
    auto value = version_into_value(version);

    std::string result = std::to_string(version.major);
    if (value & 0x0000ffffffffffff || reserve > 1)
        result = result + "." + std::to_string(version.minor);

    if (value & 0x00000000ffffffff || reserve > 2)
        result = result + "." + std::to_string(version.patch);

    if (value & 0x000000000000ffff || reserve > 3)
        result = result + "." + std::to_string(version.build);

    return result;
}

std::wstring version_into_wstring(const product_version& version, int reserve /*= 3*/)
{
    return util::conv::easy::_2wstr(version_into_string(version, reserve));
}

std::string version_into_string(uint64_t version, int reserve /*= 3*/)
{
    return version_into_string(version_from_value(version), reserve);
}

std::wstring version_into_wstring(uint64_t version, int reserve /*= 3*/)
{
    return version_into_wstring(version_from_value(version), reserve);
}

std::string version_into_string_from32(uint32_t version, int reserve /*= 3*/)
{
    return version_into_string(version_from_value32(version), reserve);
}

std::wstring version_into_wstring_from32(uint32_t version, int reserve /*= 3*/)
{
    return version_into_wstring(version_from_value32(version), reserve);
}

std::string version_into_string(
    uint16_t major, uint16_t minor, 
    uint16_t patch /*= 0*/, uint16_t build /*= 0*/, int reserve /*= 3*/)
{
    return version_into_string(version_make(major, minor, patch, build), reserve);
}

std::wstring version_into_wstring(
    uint16_t major, uint16_t minor, 
    uint16_t patch /*= 0*/, uint16_t build /*= 0*/, int reserve /*= 3*/)
{
    return version_into_wstring(version_make(major, minor, patch, build), reserve);
}

} // util