#ifdef UTILITY_DISABLE_HEADONLY
#   include "../thread_interrupt.hpp"
#endif

#if UTILITY_SUPPORT_BOOST

#include <boost/atomic.hpp>
#include <boost/thread/tss.hpp>

namespace util {
namespace interrupt {
namespace detail {

/* �̱߳��ش洢 */
boost::thread_specific_ptr<boost::atomic_int32_t> __threadlocal;

} // detail

intptr_t thread_interrupt_init()
{
    detail::__threadlocal.reset(new boost::atomic_int32_t(0));

    return reinterpret_cast<intptr_t>(detail::__threadlocal.get());
}

bool thread_interrupt_reset(intptr_t handle/* = -1*/)
{
    if (handle != -1)
    {
        if (auto flags_ptr = reinterpret_cast<boost::atomic_int32_t*>(handle))
            return !(*flags_ptr = 0);
    }
    else
    {
        if (detail::__threadlocal.get())
            return !(*detail::__threadlocal = 0);
    }

    return false;
}

int thread_interrupt_flags(intptr_t handle/* = -1*/)
{
    if (handle != -1)
    {
        if (auto flags_ptr = reinterpret_cast<boost::atomic_int32_t*>(handle))
            return *flags_ptr;
    }
    else
    {
        if (detail::__threadlocal.get())
            return *detail::__threadlocal;
    }

    return 0;
}

void thread_interrupt_request(
    intptr_t handle/* = -1*/, int flags/* = -1*/)
{
    if (handle != -1)
    {
        if (auto flags_ptr = reinterpret_cast<boost::atomic_int32_t*>(handle))
            *flags_ptr |= flags;
    }
    else
    {
        if (detail::__threadlocal.get())
            *detail::__threadlocal |= flags;
    }
}

bool thread_interrupt_requested(
    intptr_t handle/* = -1*/, int flags/* = -1*/)
{
    if (handle != -1)
    {
        if (auto flags_ptr = reinterpret_cast<boost::atomic_int32_t*>(handle))
            return !!(*flags_ptr & flags);
    }
    else
    {
        if (detail::__threadlocal.get())
            return !!(*detail::__threadlocal & flags);
    }

    return false;
}

void thread_interrupt_point(
    int flags/* = -1*/,
    const boost::any& param/* = boost::any()*/)
{
    if (thread_interrupt_requested(-1, flags))
    {
        int32_t intflags = thread_interrupt_flags(-1);
#if defined(_DEBUG) && (NTDDI_VERSION > NTDDI_WINXP)
        util::output_debug_string(
            util::sformat(
                L"thread_interrupt_point(%08x)\n"
                L"\tthrow thread_interrupted(%08x, %08x) at:\n",
                flags, intflags, param) +
            util::stack_trace(1));
#endif
        throw thread_interrupted(intflags, param);
    }
}

void thread_interrupt_trigger(
    int flags/* = -1*/,
    const boost::any& param/* = boost::any()*/)
{
#if defined(_DEBUG) && (NTDDI_VERSION > NTDDI_WINXP)
    util::output_debug_string(
        util::sformat(
            L"throw thread_interrupted(%08x, %08x) at:\n",
            flags, param) +
        util::stack_trace(1));
#endif
    throw thread_interrupted(flags, param);
}

} // interrupt
} // util

#endif // UTILITY_SUPPORT_BOOST