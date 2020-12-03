#ifndef thread_interrupt_h__
#define thread_interrupt_h__

/*
*   thread_interrupt.hpp 
*
*   v0.1 2019-06 by GuoJH
*/

#if UTILITY_SUPPORT_BOOST
#   include <boost/any.hpp>
#endif
#include <common/common_cfg.hpp>

namespace util {
namespace interrupt {

// need boost support 
#if UTILITY_SUPPORT_BOOST

/*! 
 *   线程中断异常 
 */
class thread_interrupted
{
    int32_t       __flags;
    boost::any    __param;
public:
    thread_interrupted(int32_t flags = 0, const boost::any& any = boost::any()) 
        : __flags(flags)
        , __param(any)
    {}

    int32_t flags() const { return __flags; }

    template<class _Type>
    _Type param() const 
    {
        return boost::any_cast<_Type>(__param);
    }
};

/*!
 *  /brief  线程中断初始化
 *  /return 将返回一个句柄, 可以被task_request_interrupt()使用.
 */
UTILITY_FUNCT_DECL intptr_t thread_interrupt_init();

/*!
 *  /brief  重置线程中断请求
 *  /return 返回true成功, 否则失败.
 */
UTILITY_FUNCT_DECL bool thread_interrupt_reset(intptr_t handle = -1);

/*!
 *  /brief 返回指定线程flags的值.
 *  /param handle 指定线程的中断句柄, -1 指向当前线程.
 */
UTILITY_FUNCT_DECL int thread_interrupt_flags(intptr_t handle = -1);

/*!
 *  /brief 请求指定线程中断
 *  /param handle 指定线程的中断句柄, -1 指向当前线程
 *  /param flags  判断指定的中断类型, -1 代表所有中断(其意义由应用程序定义).
 */
UTILITY_FUNCT_DECL void thread_interrupt_request(
    intptr_t handle = -1, int flags = -1);

/*
 *  /brief 返回指定线程是否被请求中断
 *  /param handle 指定线程的中断句柄, -1 指向当前线程
 *  /param flags  判断指定的中断类型, -1 代表所有中断(其意义由应用程序定义).
 */
UTILITY_FUNCT_DECL bool thread_interrupt_requested(
    intptr_t handle = -1, int flags = -1);

/*!
 *  /brief 线程中断点
 *  /param flags 代表可接受的中断类型, 若当前中断类型与可接受类型不匹配则不会触发中断.
 *  /param param 用户参数
 * 
 *  /note  判断当前线程是否被触发中断, 若是则抛出中断异常;
 */
UTILITY_FUNCT_DECL void thread_interrupt_point(
    int flags = -1,
    const boost::any& param = boost::any());

/*!
 *  /brief 主动触发一个线程中断
 *  /param flags 代表可接受的中断类型
 *  /param param 用户参数
 */
UTILITY_FUNCT_DECL void thread_interrupt_trigger(
    int flags = -1,
    const boost::any& param = boost::any());

#endif // UTILITY_SUPPORT_BOOST

} // interrupt
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/thread_interrupt.ipp"
#endif

#endif // thread_interrupt_h__
