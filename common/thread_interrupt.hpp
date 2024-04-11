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
 *   �߳��ж��쳣 
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
 *  /brief  �߳��жϳ�ʼ��
 *  /return ������һ�����, ���Ա�task_request_interrupt()ʹ��.
 */
UTILITY_FUNCT_DECL intptr_t thread_interrupt_init();

/*!
 *  /brief  �����߳��ж�����
 *  /return ����true�ɹ�, ����ʧ��.
 */
UTILITY_FUNCT_DECL bool thread_interrupt_reset(intptr_t handle = -1);

/*!
 *  /brief ����ָ���߳�flags��ֵ.
 *  /param handle ָ���̵߳��жϾ��, -1 ָ��ǰ�߳�.
 */
UTILITY_FUNCT_DECL int thread_interrupt_flags(intptr_t handle = -1);

/*!
 *  /brief ����ָ���߳��ж�
 *  /param handle ָ���̵߳��жϾ��, -1 ָ��ǰ�߳�
 *  /param flags  �ж�ָ�����ж�����, -1 ���������ж�(��������Ӧ�ó�����).
 */
UTILITY_FUNCT_DECL void thread_interrupt_request(
    intptr_t handle = -1, int flags = -1);

/*
 *  /brief ����ָ���߳��Ƿ������ж�
 *  /param handle ָ���̵߳��жϾ��, -1 ָ��ǰ�߳�
 *  /param flags  �ж�ָ�����ж�����, -1 ���������ж�(��������Ӧ�ó�����).
 */
UTILITY_FUNCT_DECL bool thread_interrupt_requested(
    intptr_t handle = -1, int flags = -1);

/*!
 *  /brief �߳��жϵ�
 *  /param flags ����ɽ��ܵ��ж�����, ����ǰ�ж�������ɽ������Ͳ�ƥ���򲻻ᴥ���ж�.
 *  /param param �û�����
 * 
 *  /note  �жϵ�ǰ�߳��Ƿ񱻴����ж�, �������׳��ж��쳣;
 */
UTILITY_FUNCT_DECL void thread_interrupt_point(
    int flags = -1,
    const boost::any& param = boost::any());

/*!
 *  /brief ��������һ���߳��ж�
 *  /param flags ����ɽ��ܵ��ж�����
 *  /param param �û�����
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
