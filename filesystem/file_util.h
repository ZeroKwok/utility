#ifndef file_util_h__
#define file_util_h__

/*
*   file_util.h
*   
*   v0.1 2018-03 by GuoJH
*   v0.2 2019-07 by GuoJH
*   v0.3 2020-12 by GuoJH
*/

#include <stdio.h>
#include <fcntl.h>
#include <string/tstring.h>
#include <platform/platform_error.h>
#include <filesystem/filesystem_cfg.h>

namespace util {

//! Define file system path, size, and error messages.
typedef uint64_t             fsize;
typedef util::tstring        fpath;
typedef util::platform_error ferror;

/*!
 *  \brief 文件时间
 * 
 *  \note  1. 使用epoch时间戳, 单位是秒, 即 UTC 1970-01-01 00:00:00到现在的秒数.
 *         2. 由于在某些类unix系统中没有文件创建时间, 取而代之的是文件状态改变时间, 
 *            故在此类系统上create_time无效, 无效时间将被设置为-1.
 */
struct ftime
{
    int64_t create_time;   //!< 创建时间, 在不支持创建时间的平台上为-1.
    int64_t access_time;   //!< 访问时间
    int64_t modify_time;   //!< 修改时间
    int64_t change_time;   //!< 改变时间, 仅类unix平台有效, 否则为-1.
};

/*!
 *  \brief 文件描述符的包装对象
 *  
 *  \note  类似于unique_ptr, fflie 赋值后转移对文件的所有权, 原对象将被设置为无效.
 *         在对象实例析构时, 如实例有效将尝试关闭打开的文件.
 */
class ffile
{
public:
#if OS_POSIX
    typedef int      native_type;
#else
    typedef intptr_t native_type;
#endif
    typedef int      open_flags;

    explicit ffile(native_type fid = -1);
    explicit ffile(native_type fid, open_flags flags);

    //! 从其他实例构造, other 将丧失对文件的所有权.
    ffile(ffile& other); 
    ~ffile();

    //! C++ 11 移动构造支持
#if __cplusplus >= 201103L || _MSVC_LANG >= 201103L
    ffile(ffile&& right);
    ffile& operator=(ffile&& right);
#endif

    //! 赋值后, right 将丧失对文件的所有权.
    ffile& operator=(ffile& right);

    bool vaild() const;
    void close(); // Different implementations

    //! 返回文件打开时的flags.
    open_flags flags() const;

    //! 返回特定于平台的原始文件描述符.
    native_type native_id() const;

    //! 与vaild()相同
    operator bool() const;

    //! 与native_id()相同
    operator native_type() const;

protected:
    open_flags  _open_flags;
    native_type _native_id;
};

/*!
 *  /brief  判断文件或目录是否存在.
 * 
 *  /return 如果文件或目录存在返回true, 不存在或者失败返回false并设置ferr
 *          (在非noexcept版本中抛出异常).
 * 
 *  /note   如果name指向一个符号链接, 则会进一步解析指向的内容, 如果指向一个不存在的文件, 将返回false.
 *          对于类unix, 该函数无视文件权限, 但是被路径中目录的执行(搜索)权限所限制.
 */
UTILITY_FUNCT_DECL bool file_exist(const fpath& name);
UTILITY_FUNCT_DECL bool file_exist(const fpath& name, ferror& ferr) noexcept;

/*!
 *  \brief 移除指定的文件
 *  
 *  \note 如果name指向一个符号链接，则该链接将被删除。
 *        对于Unix-Like, 该行为与unlink类似:
 *        1. 如果name是指向文件的最后一个链接(硬链接)，并且没有进程打开该文件，那么该文件将被删除.
 *        2. 如果name是指向文件的最后一个链接(硬链接)，但是存在进程仍然打开该文件，那么该文件将一直存在，
 *           直到最后一个引用它的文件描述符被关闭。
 */
UTILITY_FUNCT_DECL void file_remove(const fpath& name);
UTILITY_FUNCT_DECL void file_remove(const fpath& name, ferror& ferr) noexcept;

/*!
 *  \brief 拷贝文件
 * 
 *  \note  1. 如果to指向的文件已经存在, 则将覆盖该文件.
 *         2. 如果from指向符号链接, 则将进一步解析符号链接指向的内容;
 *         3. 如果to  指向符号链接, 则操作将失败.
 *         4. 只能拷贝文件不支持文件夹, 若要拷贝目录, 参考: std/boost::filesystem::copy().
 */
UTILITY_FUNCT_DECL void file_copy(const fpath& from, const fpath& to);
UTILITY_FUNCT_DECL void file_copy(const fpath& from, const fpath& to, ferror& ferr) noexcept;

/*!
 *  \brief 移动文件
 * 
 *  \note  1. 如果to指向的文件已经存在将出错.
 *         2. 优先通过rename(), 若失败则尝试copy(), 再删除from,
 *            如果复制操作失败或无法删除该文件，则删除目标文件newName以恢复旧状态。
 */
UTILITY_FUNCT_DECL void file_move(const fpath& from, const fpath& to);
UTILITY_FUNCT_DECL void file_move(const fpath& from, const fpath& to, ferror& ferr) noexcept;

/*!
 *  \brief 返回文件的大小
 * 
 *  \note  如果name指向一个符号链接, 则会进一步解析指向的内容, 若不存在则失败.
 */
UTILITY_FUNCT_DECL fsize file_size(const fpath& name);
UTILITY_FUNCT_DECL fsize file_size(const fpath& name, ferror& ferr)  noexcept;

/*!
 *  \brief 返回文件的时间信息
 *  
 *  \note  如果name指向一个符号链接, 则会进一步解析指向的内容, 若不存在则失败.
 */
UTILITY_FUNCT_DECL ftime file_time(const fpath& name);
UTILITY_FUNCT_DECL ftime file_time(const fpath& name, ferror& ferr)  noexcept;

/*!
 *  \brief 设置文件时间
 *  \param name 将改变时间的文件
 *  \param time 文件时间的结构, 如不修改某些字段将其设置为: -1.
 * 
 *  \note  1. 如果name指向一个符号链接, 则会进一步解析指向的内容, 若不存在则失败.
 *         2. 需要注意的是在不同平台中对文件时间的支持度都不一样.
 *            a. 在windows中创建,修改,访问时间可以任意设置.
 *            b. 在linux中不支持创建时间, 仅支持设置文件修改, 访问时间.
 */
UTILITY_FUNCT_DECL void file_set_time(const fpath& name, const ftime& time);
UTILITY_FUNCT_DECL void file_set_time(const fpath& name, const ftime& time, ferror& ferr) noexcept;

/*!
 *  \brief 打开指定的文件
 * 
 *  \param oflag 打开文件的标识符, 其定义在fcntl.h
 *         O_RDONLY  = 0 open for reading only.
 *         O_WRONLY  = 1 open for writing only.
 *         O_RDWR    = 2 open for reading and writing.
 * 
 *         O_APPEND      writes done at eof.
 *         O_CREAT       create and open file.
 *         O_TRUNC       open and truncate.
 *         O_EXCL        open only if file doesn't already exist.
 * 
 *  \note  1. 如果name指向符号链接, 则将进一步解析其内容指向的文件.
 *         2. 对于windows平台, 其通过CreateFile模拟POSIX open()函数的行为:
 *            在指定O_APPEND标识符时打开文件时行为与POSIX定义有差异, 前者设置文件指针与写入数据不是原子操作,
 *            而::open()函数在这里将是原子的;
 *         3. 对于POSIX平台, 其直接调用open(), 默认创建权限采用666;
 *
 *	** 以这种模式O_WRONLY | O_CREAT 打开文件, 似乎win的逻辑是追加而不是复写, 需要测试
 */
UTILITY_FUNCT_DECL ffile file_open(const fpath& name, int oflag);
UTILITY_FUNCT_DECL ffile file_open(const fpath& name, int oflag, ferror& ferr) noexcept;

//! 
//! 关闭file指向的文件
//! 
UTILITY_FUNCT_DECL void file_close(ffile& file) noexcept;

/*!
 *  \brief 从文件读取内容
 *  
 *  \note  1. 若文件实际内容小于读取内容将出错, 但缓存区仍然会被填充.
 *         2. 对于Unix-Like, 若系统调用因信号中断会继续尝试, 直到成功为止.
 */
UTILITY_FUNCT_DECL void file_read(const ffile& file, void* data_out, int size);
UTILITY_FUNCT_DECL void file_read(const ffile& file, void* data_out, int size, ferror& ferr) noexcept;

/*!
 *  \brief 将内容写入文件
 *
 *  \note  1. 对于Unix-Like, 若系统调用因信号中断会继续尝试, 直到成功为止.
 */
UTILITY_FUNCT_DECL void file_write(ffile& file, const void *data, int size);
UTILITY_FUNCT_DECL void file_write(ffile& file, const void *data, int size, ferror& ferr) noexcept;

/*!
 *  \brief 设置文件指针
 * 
 *  \param file 文件句柄
 *  \param offset 文件指针相对于whence的偏移量, 支持4GB以上大文件.
 *  \param whence 文件指针偏移量的参考位置, 取下列值之一:
 *                1. SEEK_SET  = 0 文件的开始位置
 *                2. SEEK_CUR  = 1 文件指针的当前位置
 *                3. SEEK_END  = 2 文件的末尾
 */
UTILITY_FUNCT_DECL void file_seek(ffile& file, fsize offset, int whence = SEEK_SET);
UTILITY_FUNCT_DECL void file_seek(ffile& file, fsize offset, int whence, ferror& ferr) noexcept;

//! 返回文件当前指针相对于文件开始位置的偏移量.
UTILITY_FUNCT_DECL fsize file_tell(const ffile& file);
UTILITY_FUNCT_DECL fsize file_tell(const ffile& file, ferror& ferr) noexcept;

//! 返回文件大小
UTILITY_FUNCT_DECL fsize file_size(const ffile& file);
UTILITY_FUNCT_DECL fsize file_size(const ffile& file, ferror& ferr) noexcept;

/*!
 *  \brief 返回文件时间
 * 
 *  \see   file_time(const fpath& name).
 */
UTILITY_FUNCT_DECL ftime file_time(const ffile& file);
UTILITY_FUNCT_DECL ftime file_time(const ffile& file, ferror& ferr) noexcept;

/*!
 *  \brief 设置文件时间
 * 
 *  \see   file_set_time(const fpath& name, const ftime& time).
 */
UTILITY_FUNCT_DECL void file_set_time(const ffile& file, const ftime& time);
UTILITY_FUNCT_DECL void file_set_time(const ffile& file, const ftime& time, ferror& ferr) noexcept;

/*!
 *  \brief 返回当前进程对name指向的文件是否可写
 * 
 *  \note  如果name指向符号链接, 则将进一步解析其内容指向的文件.
 */
UTILITY_FUNCT_DECL bool file_is_writable(const fpath& name);
UTILITY_FUNCT_DECL bool file_is_writable(const fpath& name, ferror& ferr) noexcept;
     
/*!
 *  \brief 递归函数目录及其内容
 * 
 *  \note  在目录内递归删除内容时, 若发生某个文件删除失败则会跳过这个文件, 继续清除下一个文件, 并且将设置ferr.
 */
UTILITY_FUNCT_DECL void directories_remove(const fpath& path);
UTILITY_FUNCT_DECL void directories_remove(const fpath& path, ferror& ferr) noexcept;

/*!
 *  \brief 递归创建目录
 *  
 *  \note  1. 在POSIX系统中新创建目录的权限为(0775 & ~umask & 0777), 至于其他模式位取决于操作系统.
 */
UTILITY_FUNCT_DECL void directories_create(const fpath& path);
UTILITY_FUNCT_DECL void directories_create(const fpath& path, ferror& ferr) noexcept;

} // util

//!
//! windows/posix 方面的扩展
//! 
#if OS_WIN
#   include <filesystem/file_win.h>
#elif OS_POSIX
#   include <filesystem/file_unix.h>
#endif

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/file_util.ipp"
#endif

#endif // file_util_h__
