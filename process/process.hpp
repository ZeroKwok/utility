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
    typedef int launch_policys;

    /// ʵ������Ч�Ķ���, ��vaild() == false;
    process();

    /*!
     *  /brief  ��pidʵ��������;
     *  /note   ͬboost.process��ͬ, �ⲻ��֤�ɹ�, ��Ϊ��Ҫ��ȷ�ķ���Ȩ��, 
     *          ����ͨ��error, vaild()��֤�Ƿ�ɹ�;
     */
    explicit process(pid_t pid, platform_error& error);

     /*!
      *  /brief  ͨ���������̵ķ�ʽʵ��������;
      *  /param  app  �ļ���, ���������ļ���ʱ, �����������·��, �������ǰĿ¼, 
      *               ���������չ��. �ò�������Ϊ��, ��ʱ����cmd����Ѱ�ҿ�ִ���ļ�;
      * 
      *  /param  cmd  �����в���, ��appΪ��, ����Ҫ������ִ���ļ�, ������ϵͳ����Ŀ¼���ҿ�ִ���ļ�,
      *               �����������չ����ٶ�.exe;
      *  /param  policy ��������, �μ�launch_policy;
      *  /param  error  ����������, �ò�����������;
      * 
      *  /note   ͬboost.process��ͬ, �ⲻ��֤�ɹ�, ����ͨ��error, vaild()��֤�Ƿ�ɹ�;
      */
    explicit process(
        const util::tstring& app, 
        const util::tstring& cmd,
        launch_policys policy,
        platform_error& error);

    /*!
     *  /brief  ���ٶ���
     *  /note   ���������Чvaild() == true, ��ͨ��std::terminate()��ֹ����.
     *          ��, �ڶ�������ǰ, �������Join()����detach(), ��Ŀ�����ڷ���;
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
     *  /brief  �ȴ����̽������߳�ʱ
     *  /param millisec -1 ��ʾ���޵ȴ�, ֱ��Ŀ�������ֹ, ������һ���Ժ���Ƶ�ʱ��;
     *  /param error       ���ò��������˴���, �������õ��ò���;
     * 
     *  /note   ���ܵȴ�����, ���򽫵õ�һ������, ���error;
     */
    bool wait_until(int millisec, platform_error& error);

    /*!
     *  /brief  ��������ֹ����
     *  /note   �ò������첽��, Ҳ����˵���÷������غ�, Ŀ����̿�����Ȼ������, 
     *          ��Ҫȷ��Ŀ������Ѿ�����, ���Ե���join()����wait_until();
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

