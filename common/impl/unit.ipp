#ifdef UTILITY_DISABLE_HEADONLY
#   include "../unit.h"
#endif

#include <cmath>
#include <vector>
#include <string/string_util.h>
#include <string/string_conv_easy.hpp>

namespace util {

std::string bytes_add_suffix(
    double bytes, int base/* = 1024*/, const std::string& suffix/* = ""*/)
{
    if(bytes < 100)
        return util::sformat("%.0fB", bytes) + suffix;

    const char* prefix[] = {"KB", "MB", "GB", "TB"};

    for (int i = 0; i < sizeof(prefix) / sizeof(const char*); ++i)
    {
        bytes /= base;
        if (std::fabs(bytes) < double(base))
            return util::sformat("%.2f", bytes) + prefix[i] + suffix;
    }

    return util::sformat("%.2fPB", bytes) + suffix;
}

std::wstring wbytes_add_suffix(
    double bytes, int base /*= 1024*/, const std::wstring& suffix /*= L""*/)
{
    return conv::easy::_2wstr(bytes_add_suffix(bytes, base, conv::easy::_2str(suffix)));
}

std::string seconds_add_suffix(
    int seconds, const std::string& suffix/* = ""*/)
{
    if (seconds < 60)
        return util::sformat("%dSec", seconds) + suffix;

    const char* prefix[] = { "Min", "Hr", "Day" };
    const float base[] = { 60.0f, 24.0f, 7.0f };

    for (int i = 0; i < sizeof(prefix) / sizeof(const char*); ++i)
    {
        seconds = int(seconds / base[i]);

        if (std::abs(seconds) < base[i]) {
            return util::sformat("%d", seconds) + prefix[i] + suffix;
        }
    }

    return util::sformat("%dWK", seconds) + suffix;
}

std::wstring wseconds_add_suffix(
    int seconds, const std::wstring& suffix/* = L""*/)
{
    return conv::easy::_2wstr(seconds_add_suffix(seconds, conv::easy::_2str(suffix)));
}

std::string duration_format(
    int seconds, const std::string& separator/* = ":"*/)
{
    const int preHour = 60 * 60;
    const int preMinute = 60;

    int hours   = seconds / preHour;
    int minutes = (seconds - (hours * preHour)) / 60;
    int remain  = seconds % preMinute;

    std::vector<int> node;
    if (hours > 0)
        node.push_back(hours);

    node.push_back(minutes);
    node.push_back(remain);

    std::string format;
    for (size_t i = 0; i < node.size(); ++i)
    {
        format += sformat("%02d", node[i]);
        if ((i + 1) < node.size())
            format += separator;
    }

    return format;
}

std::wstring wduration_format(
    int seconds, const std::wstring& separator/* = L":"*/)
{
    return conv::easy::_2wstr(duration_format(seconds, conv::easy::_2str(separator)));
}

std::string duration_ms_format(
    int milliseconds, const std::string& separator/* = ":"*/)
{
    std::string left, right;
    if (milliseconds > 1000)
        left = duration_format(milliseconds / 1000, separator);

    auto remain = milliseconds % 1000;
    if (remain > 0)
        right = std::to_string(remain);

    if (left.empty())
        return "0." + right;

    if (right.empty())
        return left;

    return left + "." + right;
}

std::wstring wduration_ms_format(
    int milliseconds, const std::wstring& separator/* = L":"*/)
{
    return conv::easy::_2wstr(duration_ms_format(milliseconds, conv::easy::_2str(separator)));

}

} // util