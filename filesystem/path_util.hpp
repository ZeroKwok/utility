#ifndef path_util_h__
#define path_util_h__

/*
*   path_util.hpp
*
*   v0.1 2018-3 by GuoJH
*   v0.2 2019-7 by GuoJH
*/

#include <filesystem/file_util.hpp>
#include <filesystem/filesystem_cfg.hpp>

namespace util {

/*
*   ·����������
*/
UTILITY_FUNCT_DECL fpath path_from_module(intptr_t module = 0);
UTILITY_FUNCT_DECL fpath path_from_module(intptr_t module, ferror& ferr);
UTILITY_FUNCT_DECL fpath path_from_module_dir(intptr_t module = 0);
UTILITY_FUNCT_DECL fpath path_from_module_dir(intptr_t module, ferror& ferr);
UTILITY_FUNCT_DECL fpath path_from_module_dir(const fpath& stem);
UTILITY_FUNCT_DECL fpath path_from_module_dir(const fpath& stem, ferror& ferr);
UTILITY_FUNCT_DECL fpath path_from_temp();
UTILITY_FUNCT_DECL fpath path_from_temp(ferror& ferr);
UTILITY_FUNCT_DECL fpath path_from_sysdir(int flag = 0);
UTILITY_FUNCT_DECL fpath path_from_sysdir(int flag, ferror& ferr);

/*
*   ��explorer�д��ļ��в�ѡ��ָ�����ļ�
*/
UTILITY_FUNCT_DECL void path_open_with_explorer(const fpath& path, bool select = true);
UTILITY_FUNCT_DECL void path_open_with_explorer(const fpath& path, bool select, ferror& ferr);

/*
*	�ж��Ƿ�ΪRootĿ¼
*/
UTILITY_FUNCT_DECL bool path_is_root(const fpath& path);

/*
*	�ж��Ƿ�ΪUNC·��
*/
UTILITY_FUNCT_DECL bool path_is_unc(const fpath& path);

/*
*	�ж��Ƿ�ΪURL·��
*/
UTILITY_FUNCT_DECL bool path_is_url(const fpath& path);

/*
*	�ж�·���Ƿ�ΪWindows���
*   1. ��⵽�������� :
*   2. ��⵽�ָ���   \
*/
UTILITY_FUNCT_DECL bool path_is_win_style(const fpath& path);

/*
*	�ж�·���Ƿ�ΪLinux���
*   1. ��⵽��Ŀ¼ /
*   2. ��⵽�ָ��� / ��Ϊ ��Windows ���
*/
UTILITY_FUNCT_DECL bool path_is_linux_style(const fpath& path);

/*
*	�ж�·���Ƿ�ΪUNC���
*   1. ��⵽ǰ׺     \\
*   2. δ��⵽�ָ��� /
*/
UTILITY_FUNCT_DECL bool path_is_unc_style(const fpath& path);

/*
*	�ж��Ƿ�ΪԶ��·�� (�����ж϶Ͽ����ӵ�����������)
*/
UTILITY_FUNCT_DECL bool path_is_remote(const fpath& path);

/*
*	�ж�·���Ƿ��д
*   ��ָ��·��������, �����ϲ��ҵ�һ�����ڵĸ�Ŀ¼���ж��Ƿ��д
*/
UTILITY_FUNCT_DECL bool path_is_writable(const fpath& path);
UTILITY_FUNCT_DECL bool path_is_writable(const fpath& path, ferror& ferr);

/*
*	·������
*/
UTILITY_FUNCT_DECL std::string  path_append(const std::string& path, const std::string& stem_1);
UTILITY_FUNCT_DECL std::wstring path_append(const std::wstring& path, const std::wstring& stem_1);
UTILITY_FUNCT_DECL std::string  path_append(const std::string& path, const std::string& stem_1, const std::string& stem_2);
UTILITY_FUNCT_DECL std::wstring path_append(const std::wstring& path, const std::wstring& stem_1, const std::wstring& stem_2);

/*
*	���rootĿ¼
*   ����·��: ������    C:\Program Files\windows    -> C:\
*   ����·��: UNC��Ŀ¼ \\192.168.0.13\share\folder -> \\192.168.0.13\share
*/
UTILITY_FUNCT_DECL std::string  path_find_root(const std::string& path);
UTILITY_FUNCT_DECL std::wstring path_find_root(const std::wstring& path);
UTILITY_FUNCT_DECL std::string  path_find_parent(const std::string& path);
UTILITY_FUNCT_DECL std::wstring path_find_parent(const std::wstring& path);
UTILITY_FUNCT_DECL std::string  path_find_filename(const std::string& path);
UTILITY_FUNCT_DECL std::wstring path_find_filename(const std::wstring& path);
UTILITY_FUNCT_DECL std::string  path_find_extension(const std::string& path, bool hasdot = true, bool lower = true);
UTILITY_FUNCT_DECL std::wstring path_find_extension(const std::wstring& path, bool hasdot = true, bool lower = true);

/*
*   �ļ����ü�, ʹ�ļ����Ϸ���
*   filename ���ļ���, ����Ϊ·��
*/
UTILITY_FUNCT_DECL std::string  path_filename_trim(const std::string& filename);
UTILITY_FUNCT_DECL std::wstring path_filename_trim(const std::wstring& filename);

/*
*   ����һ����������ļ���
*   etc. log.txt -> log(1).txt
*                   log(1).txt -> log(2).txt
*/
UTILITY_FUNCT_DECL std::string  path_filename_increment(const std::string& filename);
UTILITY_FUNCT_DECL std::wstring path_filename_increment(const std::wstring& filename);

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/path_util.ipp"
#endif

#endif // path_util_h__
