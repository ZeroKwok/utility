#ifdef UTILITY_DISABLE_HEADONLY
#   include "../unit.h"
#endif

#include <cmath>
#include <vector>
#include <string/string_util.h>
#include <string/string_conv_easy.hpp>

namespace util {

std::string bytes_add_suffix(
    double bytes, int base/* = 1024*/, const std::string suffix/* = ""*/)
{
    if(bytes < 100)
        return util::sformat("%.0fB", bytes) + suffix;

    const char* prefix[] = {"kB", "MB", "GB", "TB"};

    for (int i = 0; i < sizeof(prefix) / sizeof(const char*); ++i)
    {
        bytes /= base;
        if (std::fabs(bytes) < double(base))
            return util::sformat("%.2f", bytes) + prefix[i] + suffix;
    }

    return util::sformat("%.2fPB", bytes) + suffix;
}

std::wstring wbytes_add_suffix(
    double bytes, int base /*= 1024*/, const std::wstring suffix /*= L""*/)
{
    return conv::easy::_2wstr(bytes_add_suffix(bytes, base, conv::easy::_2str(suffix)));
}

std::string seconds_add_suffix(
    int sec, const std::string suffix/* = ""*/)
{
    if (sec < 60)
        return util::sformat("%dSec", sec) + suffix;

    const char* prefix[] = { "Min", "Hr", "Day" };
    const float base[] = { 60.0f, 24.0f, 7.0f };

    for (int i = 0; i < sizeof(prefix) / sizeof(const char*); ++i)
    {
        sec = int(sec / base[i]);

        if (std::abs(sec) < base[i]) {
            return util::sformat("%d", sec) + prefix[i] + suffix;
        }
    }

    return util::sformat("%dWK", sec) + suffix;
}

std::wstring wseconds_add_suffix(
    int sec, const std::wstring suffix/* = L""*/)
{
    return conv::easy::_2wstr(seconds_add_suffix(sec, conv::easy::_2str(suffix)));
}

std::string duration_format(
    int sec, const std::string separator/* = ":"*/)
{
    const int preHour = 60 * 60;
    const int preMinute = 60;

    int hours = sec / preHour;
    int minutes = (sec - (hours * preHour)) / 60;
    int seconds = sec % preMinute;

    std::vector<int> node;
    if (hours > 0)
        node.push_back(hours);

    node.push_back(minutes);
    node.push_back(seconds);

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
    int sec, const std::wstring separator/* = L":"*/)
{
    return conv::easy::_2wstr(duration_format(sec, conv::easy::_2str(separator)));
}

} // util