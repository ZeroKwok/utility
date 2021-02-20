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
    typedef int launch_policys;

    /// 实例化无效的对象, 即vaild() == false;
    process();

    /*!
     *  /brief  从pid实例化对象;
     *  /note   同boost.process相同, 这不保证成功, 因为需要正确的访问权限, 
     *          可以通过error, vaild()验证是否成功;
     */
    explicit process(pid_t pid, platform_error& error);

     /*!
      *  /brief  通过创建进程的方式实例化对象;
      *  /param  app  文件名, 当不包含文件名时, 不会检索搜索路径, 会检索当前目录, 
      *               必须包含扩展名. 该参数可以为空, 此时将从cmd参数寻找可执行文件;
      * 
      *  /param  cmd  命令行参数, 若app为空, 则需要包含可执行文件, 这里会从系统搜索目录查找可执行文件,
      *               如果不包含扩展名则假定.exe;
      *  /param  policy 启动策略, 参见launch_policy;
      *  /param  error  若发生错误, 该参数将被设置;
      * 
      *  /note   同boost.process相同, 这不保证成功, 可以通过error, vaild()验证是否成功;
      */
    explicit process(
        const util::tstring& app, 
        const util::tstring& cmd,
        launch_policys policy,
        platform_error& error);

    /*!
     *  /brief  销毁对象
     *  /note   如果对象有效vaild() == true, 将通过std::terminate()终止进程.
     *          即, 在对象销毁前, 必须调用Join()或者detach(), 其目的在于防呆;
     */
    ~process();

    pid_t id() const;
    native_handle_t native_handle() const;

    int  exit_code() const;
    bool valid() const;

    void detach();
    void join();
    bool joinable();
    bool running();

     /*!
     *  /brief  等待进程结束或者超时
     *  /param millisec -1 表示无限等待, 直到目标进程终止, 否则是一个以毫秒计的时间;
     *  /param error       若该操作发生了错误, 将会设置到该参数;
     * 
     *  /note   不能等待自身, 否则将得到一个错误, 检查error;
     */
    bool wait_until(int millisec, platform_error& error);

    /*!
     *  /brief  无条件终止进程
     *  /note   该操作是异步的, 也就是说当该方法返回后, 目标进程可能仍然在运行, 
     *          若要确保目标进程已经结束, 可以调用join()或者wait_until();
     */
    void terminate(platform_error& error);

protected:
    native_handle_t _handle;
};

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/process.ipp"
#endif

#endif // process_h__

