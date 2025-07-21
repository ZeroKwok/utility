// This file is part of the utility project.
// Copyright (c) 2018-2025 zero <zero.kwok@foxmail.com>
//
// For the full copyright and license information, please view the LICENSE
// file that was distributed with this source code.
//
// v0.1 2018-03
// v0.2 2019-07
// v0.3 2021-01
// v0.4 2024-06

#ifndef filesystem_h__
#define filesystem_h__

#include "config.h"
#include "fs/file.h"
#include "fs/path.h"

#include <optional>
#include <filesystem>
#include <stdio.h>
#include <fcntl.h>

namespace UTILITY_NAMESPACE {
namespace fs {

// 导入 std::filesystem;
using std::filesystem::filesystem_error;
using std::filesystem::copy_options;
using std::filesystem::copy;
using std::filesystem::copy_file;
using std::filesystem::copy_symlink;
using std::filesystem::rename;

using std::filesystem::create_directory;
using std::filesystem::create_directories;
using std::filesystem::create_hard_link;
using std::filesystem::create_symlink;
using std::filesystem::create_directory_symlink;

using std::filesystem::remove;
using std::filesystem::remove_all;

using std::filesystem::exists;
using std::filesystem::file_size;
using std::filesystem::resize_file;

using std::filesystem::space_info;
using std::filesystem::space;

using std::filesystem::file_time_type;
using std::filesystem::last_write_time;

using std::filesystem::hard_link_count;
using std::filesystem::read_symlink;

using std::filesystem::file_type;
using std::filesystem::file_status;
using std::filesystem::status;
using std::filesystem::symlink_status;

using std::filesystem::is_block_file;
using std::filesystem::is_character_file;
using std::filesystem::is_directory;
using std::filesystem::is_empty;
using std::filesystem::is_fifo;
using std::filesystem::is_other;
using std::filesystem::is_regular_file;
using std::filesystem::is_socket;
using std::filesystem::is_symlink;
using std::filesystem::status_known;

using std::filesystem::directory_options;
using std::filesystem::directory_iterator;
using std::filesystem::recursive_directory_iterator;

using std::filesystem::perms;
using std::filesystem::perm_options;
using std::filesystem::permissions;

/*!
 *  \brief 文件的不透明对象
 */
typedef struct _file* fptr;
typedef uint64_t      size;
typedef int64_t       offset;

/*!
 *  \brief 打开或创建指定的文件
 * 
 *  \param name 文件名, 如果是相对路径则相对于进程的当前目录.
 *  \param flag 打开文件的标识符(定义在 fcntl.h)
 *         - O_RDONLY: 只读模式打开
 *         - O_WRONLY: 只写模式打开
 *         - O_RDWR:   读写模式打开
 *         - O_APPEND: 追加写入到文件末尾
 *         - O_CREAT: 如果文件不存在，则创建文件
 *         - O_TRUNC: 打开并截断文件 (必须有写权限)
 *         - O_EXCL: 仅在文件不存在时打开文件
 * 
 *  \param error 错误发生时, 将存储具体的错误信息(std::system_category).
 *  \return 失败返回 nullptr, 成功返回文件的不透明对象, 需要显示通过 file_close() 关闭文件.
 * 
 *  \note  1. 仅支持二进制模式(O_BINARY)、低级、无缓冲I/O.
 *         2. 如果 name 指向符号链接, 则将进一步解析指向的文件.
 *         3. 如果 没有 error 参数, 那么错误时将抛出异常(filesystem_error).
 *         4. 对于 Windows 平台, 其通过 CreateFile() 模拟 POSIX open() 函数的行为.
 */
UTILITY_API fptr open(const path& name, int flag, int mode = 0664);
UTILITY_API fptr open(const path& name, int flag, int mode, std::error_code& error) noexcept;

/*!
 *  \brief 关闭file指向的文件
 */
UTILITY_API void close(const fptr& file);
UTILITY_API void close(const fptr& file, std::error_code& error) noexcept;

/*!
 *  \brief 读取文件, 无应用层缓冲的低级 I/O
 *  \param error 错误发生时, 将存储具体的错误信息(std::system_category).
 *  \return 表示实际读取的字节数
 *  \note  1. 若文件实际内容小于要读取的内容不视为错误, 填充对应的缓冲区后返回实际读取的字节数.
 *         2. 对于 Unix-Like, 若系统调用因信号中断会继续尝试, 直到成功为止.
 */
UTILITY_API size read(const fptr& file, void* data, size len);
UTILITY_API size read(const fptr& file, void* data, size len, std::error_code& error) noexcept;

/*!
 *  \brief 写入文件, 无应用层缓冲的低级 I/O
 *  \return 返回实际写入的字节数.
 *  \note  对于 Unix-Like, 若系统调用因信号中断会继续尝试, 直到成功为止.
 */
UTILITY_API size write(const fptr& file, const void *data, size len);
UTILITY_API size write(const fptr& file, const void *data, size len, std::error_code& error) noexcept;

/*!
 *  \brief 设置文件指针
 * 
 *  \param file 文件句柄
 *  \param offset 文件指针相对于 whence 的偏移位置（以字节为单位）, 支持大文件.
 *  \param whence 文件指针偏移位置的参考点, 取下列值之一:
 *                1. SEEK_SET  = 0 文件的开始位置
 *                2. SEEK_CUR  = 1 文件指针的当前位置
 *                3. SEEK_END  = 2 文件的末尾
 *  \return 返回新的从文件开始的偏移位置（以字节为单位）, 若发生错误则返回 -1.
 */
UTILITY_API offset seek(const fptr& file, offset offset, int whence = SEEK_SET);
UTILITY_API offset seek(const fptr& file, offset offset, int whence, std::error_code& error) noexcept;

/*!
 *  \brief 查询文件当前指针相对于文件开始位置的偏移量
 */
UTILITY_API offset tell(const fptr& file);
UTILITY_API offset tell(const fptr& file, std::error_code& error) noexcept;

/*!
 *  \brief 查询文件的大小
 *  \see  参考: std::filesystem::file_size()
 */
UTILITY_API size file_size(const fptr& file);
UTILITY_API size file_size(const fptr& file, std::error_code& error) noexcept;

/*!
 *  \brief 文件时间
 */
struct ftime
{
    std::optional<file_time_type> last_write;  // 最后修改时间
    std::optional<file_time_type> last_access; // 最后访问时间
    std::optional<file_time_type> creation;    // 创建时间(如果支持)
    std::optional<file_time_type> status;      // 状态改变时间(如果支持)
};

/*!
 *  \brief 返回指定文件的文件时间
 *  \note  跟随符号链接
 *  \see   std::filesystem::file_time_type
 */
UTILITY_API ftime file_time(const fptr& file);
UTILITY_API ftime file_time(const fptr& file, std::error_code& error) noexcept;
UTILITY_API ftime file_time(const path& filename);
UTILITY_API ftime file_time(const path& filename, std::error_code& error) noexcept;

/*!
 *  \brief 设置文件时间
 *  \note  跟随符号链接
 *  \see   std::filesystem::last_write_time
 */
UTILITY_API void file_time(const fptr& file, const ftime& time);
UTILITY_API void file_time(const fptr& file, const ftime& time, std::error_code& error) noexcept;
UTILITY_API void file_time(const path& filename, const ftime& time);
UTILITY_API void file_time(const path& filename, const ftime& time, std::error_code& error) noexcept;

/*!
 *  \brief 返回当前进程对 name 指向的文件是否可写
 * 
 *  \note  如果 name 指向符号链接, 则将进一步解析其内容指向的文件.
 *         如果 name 不存在, 则返回 false, 并设置 error.
 */
UTILITY_API bool is_writable(const path& name);
UTILITY_API bool is_writable(const path& name, std::error_code& error) noexcept;

} // fs
} // UTILITY_NAMESPACE

// 引入缩写的名称空间
namespace UTILITY_FS_NAMESPACE { using namespace UTILITY_NAMESPACE::fs; }

#endif // filesystem_path_h__
