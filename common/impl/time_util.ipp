#ifdef UTILITY_DISABLE_HEADONLY
#   include "../time_util.hpp"
#endif

#include <time.h>
#include <windows.h>
#include <common/simple_lock.hpp>
#include <string/string_util.hpp>
#include <string/string_conv_easy.hpp>

#ifdef OS_WIN
#   pragma comment(lib, "Winmm.lib")
#endif

namespace util {
namespace detail {

    // Accumulation of time lost due to rollover (in milliseconds).
    int64_t __rollover_ms = 0;

    // The last timeGetTime value we saw, to detect rollover.
    DWORD __last_seen_now = 0;

    // Lock protecting rollover_ms and last_seen_now.
    // Note: this is a global object, and we usually avoid these. However, the time
    // code is low-level, and we don't want to use Singletons here (it would be too
    // easy to use a Singleton without even knowing it, and that may lead to many
    // gotchas). Its impact on startup time should be negligible due to low-level
    // nature of time code.
    simple_lock __rollover_lock;

    // We use timeGetTime() to implement TimeTicks::Now().  This can be problematic
    // because it returns the number of milliseconds since Windows has started,
    // which will roll over the 32-bit value every ~49 days.  We try to track
    // rollover ourselves, which works if TimeTicks::Now() is called at least every
    // 49 days.
    inline int64_t rollover_protected_now()
    {
        auto_lock locked(__rollover_lock);

        // We should hold the lock while calling tick_function to make sure that
        // we keep last_seen_now stay correctly in sync.
        DWORD now = ::timeGetTime();

        if (now < __last_seen_now)
            __rollover_ms += DWORD(0x100000000);  // ~49.7 days.
        __last_seen_now = now;

        return int64_t(now + __rollover_ms);
    }

} // detail


int64_t time_ticks_now()
{
    return detail::rollover_protected_now();
}

void timezone_information(timezone_t& zone)
{
    time_t time_utc;
    struct tm tm_local, tm_gmt;

    // Get the UTC time  
    time(&time_utc);

    // Get the local time  
#ifdef OS_LINUX
    // Use localtime_r for threads safe for Linux 
    localtime_r(&time_utc, &tm_local);
#else
    localtime_s(&tm_local, &time_utc);
#endif

    // Change it to Greenwich Mean Time(GMT) tm
#ifdef OS_LINUX
    gmtime_r(&time_utc, &tm_gmt);
#else
    gmtime_s(&tm_gmt, &time_utc);
#endif

    time_t time_local = mktime(&tm_local);
    time_t time_gmt = mktime(&tm_gmt);

    zone.duration = int(time_local - time_gmt);
    zone.hours    = zone.duration / 3600;
    zone.minutes  = (zone.duration % 3600) / 60;
}

std::string time_gmt()
{
    struct tm tm_local = { 0 };
    time_t time_utc = time(0);

    localtime_s(&tm_local, &time_utc);

    char strTime[20] = { 0 };
    strftime(&strTime[0], sizeof strTime, "%Y-%m-%d %H:%M:%S", &tm_local);

    timezone_t zone = {0};
    timezone_information(zone);

    char strZone[30] = { 0 };
    sprintf_s(&strZone[0], sizeof strZone, " %s%02d%02d", 
        zone.duration < 0 ? "-" : "+", zone.hours, zone.minutes);

    return std::string(strTime) + strZone;
}

std::wstring wtime_gmt()
{
    return conv::easy::_2wstr(time_gmt());
}

// localtime_s版本time 不能为负数, 即不能格式化epoch纪元之前的时间;
// boost      版本time 不能为负数, 受导致utc_to_local;
std::string format_epoch(
    time_t time, const char* format/* = "%d-%m-%Y %H:%M, %a"*/)
{
    tm localtime = {0, 0, 0, 1, 0, 70,};

#ifdef CONFIG_SUPPORT_BOOST
    try
    {
        //                                                 from_time_t 不支持64位时间戳
        //boost::posix_time::ptime pt = boost::posix_time::from_time_t(time);
        boost::posix_time::ptime pt(boost::gregorian::date(1970, 1, 1));
                                 pt += boost::posix_time::time_duration(0, 0, time);
                                 pt =  boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(pt);        
        localtime = boost::posix_time::to_tm(pt);
    }
    catch(...)
    {
        util::output_debug_string("format_epoch: unhandled exception");
    }
#else

    if (localtime_s(&localtime, &time) != 0)
        return std::string();
#endif
    char buffer[128] = {0};
    if (strftime(buffer, 128, format, &localtime) > 0)
        return buffer;

    return std::string();
}

std::wstring wformat_epoch(
    time_t time, const wchar_t* format/* = L"%d-%m-%Y %H:%M, %a"*/)
{
    return conv::easy::_2wstr(format_epoch(time, conv::easy::_2str(format).data()));
}

#ifdef OS_WIN
int64_t epoch_from_filetime(FILETIME ft)
{
    // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
    ULARGE_INTEGER  epoch;
    epoch.LowPart = 0xD53E8000;
    epoch.HighPart = 0x019DB1DE;

    ULARGE_INTEGER ts;
    ts.LowPart = ft.dwLowDateTime;
    ts.HighPart = ft.dwHighDateTime;
    ts.QuadPart -= epoch.QuadPart;

    return ts.QuadPart / 10; // microsecond
}
#endif

} // util