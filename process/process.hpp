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
 *  /brief  �ṩ�������ص���չ����:
 *  <ul>
 *      <li> ͨ������hide_window����, ���������Ƿ���ʾ��������;
 *      <li> ͨ������elevate_privileges����, ���������Ƿ��Թ���Ա��Ȩģʽ����;
 *      <li> ͨ������depress_privileges����, ���������Ƿ��Ա�׼��Ȩģʽ����;
 *  <ul>
 *  /note   ������Ƶ�Ŀ�ǲ���boost::process���Ǵ���, ����Ӧ��ʱ���ȿ���boost;
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
        defaulted           = 0x00,     ///< Ĭ��
        hide_window         = 0x01,     ///< ���ش���
        with_shell          = 0x02,     ///< shell

        elevate_privileges  = 0x04,     ///< ����Ȩ��
        depress_privileges  = 0x08,     ///< ����Ȩ��
    };

    /// ʵ������Ч�Ķ���, ��vaild() == false;
    process();

    /*!
     *  /brief  ��pidʵ��������;
     *  /note   ͬboost.process��ͬ, �ⲻ��֤�ɹ�, ��Ϊ��Ҫ��ȷ�ķ���Ȩ��, 
     *          ����ͨ��vaild()��֤�Ƿ�ɹ�;
     */
    explicit process(pid_t pid, platform_error& error);

     /*!
      *  /brief  ͨ���������̵ķ�ʽʵ��������;
      *  /note   ͬboost.process��ͬ, �ⲻ��֤�ɹ�, ����ͨ��vaild()��֤�Ƿ�ɹ�;
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
