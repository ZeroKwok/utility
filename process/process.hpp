#ifndef process_h__
#define process_h__

/*
*   process.hpp 
*   v0.1 2018-04 by GuoJH
*   v0.2 2020-10 by GuoJH
*/

#include <string/tstring.hpp>
#include <process/process_cfg.hpp>
#include <platform/platform_error.hpp>

namespace util {

/*!
 *  /brief  提供与进程相关的扩展操作:
 *  <ul>
 *      <li> 通过传递hide_window策略, 决定进程是否显示窗口启动;
 *      <li> 通过传递elevate_privileges策略, 决定进程是否以管理员特权模式启动;
 *      <li> 通过传递depress_privileges策略, 决定进程是否以标准特权模式启动;
 *  <ul>
 *  /note   此类设计的目是补充boost::process而非代替, 故在应用时优先考虑boost;
 */
class process
{
    process(const process&);
    process& operator=(const process&);
public:
    typedef unsigned long   pid_t;
    typedef intptr_t        native_handle_t;

    enum launch_policy
    {
        defaulted           = 0x00,     ///< 默认
        hide_window         = 0x01,     ///< 隐藏窗口
        with_shell          = 0x02,     ///< shell

        elevate_privileges  = 0x04,     ///< 提升权限
        depress_privileges  = 0x08,     ///< 降低权限
    };

    /// 实例化无效的对象, 即vaild() == false;
    process();

    /*!
     *  /brief  从pid实例化对象;
     *  /note   同boost.process相同, 这不保证成功, 因为需要正确的访问权限, 
     *          可以通过vaild()验证是否成功;
     */
    explicit process(pid_t pid, platform_error& error);

     /*!
      *  /brief  通过创建进程的方式实例化对象;
      *  /note   同boost.process相同, 这不保证成功, 可以通过vaild()验证是否成功;
      */
    explicit process(
        const util::tstring& app, 
        const util::tstring& args,
        platform_error& error,
        launch_policy policy = defaulted);

    ~process();

    pid_t id() const;
    native_handle_t native_handle() const;

    int  exit_code() const;
    void detach();
    void join();
    bool joinable();
    bool valid() const;
    bool running();
    bool wait_until(int millisec = -1);
    void terminate(platform_error& error);

protected:
    native_handle_t _handle;
};



} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/process.ipp"
#endif

#endif // process_h__
