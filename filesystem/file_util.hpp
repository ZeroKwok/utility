#ifndef file_util_h__
#define file_util_h__

/*
*   file_util.hpp 
*   
*   v0.1 2018-3 by GuoJH
*   v0.2 2019-7 by GuoJH
*/

#include <fcntl.h>
#include <string/tstring.hpp>
#include <filesystem/filesystem_cfg.hpp>

namespace util {

typedef util::tstring      fpath, fpath;
typedef unsigned long long fsize;

/*
*	file system error
*/
class ferror
{
public:
    typedef int             fsecode_type;
    typedef std::string     fsemsgs_type;

    ferror(fsecode_type code = 0, const fsemsgs_type& desc = fsemsgs_type())
        : _code(code)
        , _desc(desc)
    {}

    void clear()
    {
        _code = 0;
        _desc = fsemsgs_type();
    }

    /*
    *	�����Ƿ����˴���
    *   �����������ƽ̨�Ĵ������0, ����ʾû�з�������
    */
    operator bool() const
    {
        return _code != 0;
    }

    fsecode_type code() const
    {
        return _code;
    }

    fsemsgs_type error_description() const
    {
        return _desc;
    }

    /*
    *	���ش���������ϸ��Ϣ
    *   ���ݵ�ǰƽ̨����������������������
    */
    fsemsgs_type error_message() const; // Different implementations

    fsemsgs_type print() const;

private:
    fsecode_type _code;     //ϵͳ�������
    fsemsgs_type _desc;     //����������Ϣ, ���Ǵ�����������
};

UTILITY_FUNCT_DECL bool file_exist(const fpath& name);
UTILITY_FUNCT_DECL bool file_exist(const fpath& name, ferror& ferr);

UTILITY_FUNCT_DECL void file_remove(const fpath& name);
UTILITY_FUNCT_DECL void file_remove(const fpath& name, ferror& ferr);

UTILITY_FUNCT_DECL void file_copy(const fpath& from, const fpath& to);
UTILITY_FUNCT_DECL void file_copy(const fpath& from, const fpath& to, ferror& ferr);

UTILITY_FUNCT_DECL void file_move(const fpath& from, const fpath& to);
UTILITY_FUNCT_DECL void file_move(const fpath& from, const fpath& to, ferror& ferr);

UTILITY_FUNCT_DECL fsize file_size(const fpath& name);
UTILITY_FUNCT_DECL fsize file_size(const fpath& name, ferror& ferr);

/*
*	file time
*   ʹ��epochʱ���, ��λ��΢��
*   ��(UTC)1970-01-01 00:00:00�����ڵ�΢����
*/
struct ftime
{
    uint64_t create_time;   /* ����ʱ�� */
    uint64_t access_time;   /* ����ʱ�� */
    uint64_t modify_time;   /* �޸�ʱ�� */
};

UTILITY_FUNCT_DECL ftime file_time(const fpath& name);
UTILITY_FUNCT_DECL ftime file_time(const fpath& name, ferror& ferr);

class ffile
{
public:
    typedef int      open_flags;
    typedef intptr_t native_type;

    ffile(native_type fid = -1)
        : _native_id(fid)
        , _open_flags(0)
    {}

    ffile(native_type fid, open_flags flags)
        : _native_id(fid)
        , _open_flags(flags)
    {}

    ffile(ffile& other)
        : _native_id(other._native_id)
        , _open_flags(other._open_flags)
    {
        other._native_id = -1;
    }

    ~ffile() 
    { 
        close(); 
    };

    ffile& operator=(ffile& right)
    {
        close();

        _native_id  = right._native_id;
        _open_flags = right._open_flags;

        right._native_id = -1;
        right._open_flags = 0;
        return *this;
    }

    void close(); // Different implementations

    open_flags flags() const
    {
        return _open_flags;
    }

    native_type native_id() const
    {
        return _native_id;
    }

    bool vaild() const
    {
        return _native_id != -1 &&
               _native_id != 0;
    }

protected:
    open_flags  _open_flags;
    native_type _native_id;
};

//!< O_RDONLY      0x0000  // open for reading only
//!< O_WRONLY      0x0001  // open for writing only
//!< O_RDWR        0x0002  // open for reading and writing
//!<
//!< O_APPEND      0x0008  // writes done at eof
//!< O_CREAT       0x0100  // create and open file
//!< O_TRUNC       0x0200  // open and truncate
//!< O_EXCL        0x0400  // open only if file doesn't already exist

UTILITY_FUNCT_DECL ffile file_open(const fpath& name, int oflag);
UTILITY_FUNCT_DECL ffile file_open(const fpath& name, int oflag, ferror& ferr);
UTILITY_FUNCT_DECL void  file_close(ffile& file);

UTILITY_FUNCT_DECL void file_read(const ffile& file, void* data_out, int size);
UTILITY_FUNCT_DECL void file_read(const ffile& file, void* data_out, int size, ferror& ferr);

UTILITY_FUNCT_DECL void file_write(ffile& file, const void *data, int size);
UTILITY_FUNCT_DECL void file_write(ffile& file, const void *data, int size, ferror& ferr);

/*
*   Locks the specified file for exclusive access by the calling process.
*   @1 The file handle by the file_open returned.
*   @2 The 64 bit of the starting byte offset in the file where the lock should begin.
*   @3 The 64 bit of the length of the byte range to be locked.
*   @r If the function succeeds, the return true.
*
*   Note:
*   Locking a region of a file gives the threads of the locking process exclusive access to
*   the specified region using this file handle. If the file handle is inherited by a process
*   created by the locking process, the child process is not granted access to the locked region.
*   If the locking process opens the file a second time, it cannot access the specified region
*   through this second handle until it unlocks the region.
*/
UTILITY_FUNCT_DECL bool file_lock(ffile& file, fsize offset, fsize length);
UTILITY_FUNCT_DECL bool file_lock(ffile& file, fsize offset, fsize length, ferror& ferr);

/*
*   Unlocks a region in an open file. Unlocking a region enables other processes to access the region.
*   @1 The file handle by the file_open returned.
*   @2 The 64 bit of the starting byte offset in the file where the locked region begins.
*   @3 The 64 bit of the length of the byte range to be locked.
*   @r If the function succeeds, the return true.
*
*   Note:
*   If a process terminates with a portion of a file locked or closes a file that has outstanding
*   locks, the locks are unlocked by the operating system. However, the time it takes for the
*   operating system to unlock these locks depends upon available system resources. Therefore,
*   it is recommended that your process explicitly unlock all files it has locked when it terminates.
*   If this is not done, access to these files may be denied if the operating system has not yet
*   unlocked them.
*/
UTILITY_FUNCT_DECL bool file_unlock(ffile& file, fsize offset, fsize length);
UTILITY_FUNCT_DECL bool file_unlock(ffile& file, fsize offset, fsize length, ferror& ferr);

/*
*	�����ļ�ָ��
*   SEEK_SET = 0, (FILE_BEGIN  )
*   SEEK_CUR = 1, (FILE_CURRENT)
*   SEEK_END = 2, (FILE_END    )
*/
UTILITY_FUNCT_DECL void file_seek(ffile& file, fsize pos, int flag = SEEK_SET);
UTILITY_FUNCT_DECL void file_seek(ffile& file, fsize pos, int flag, ferror& ferr);

UTILITY_FUNCT_DECL fsize file_tell(const ffile& file);
UTILITY_FUNCT_DECL fsize file_tell(const ffile& file, ferror& ferr);

UTILITY_FUNCT_DECL fsize file_size(const ffile& file);
UTILITY_FUNCT_DECL fsize file_size(const ffile& file, ferror& ferr);

UTILITY_FUNCT_DECL ftime file_time(const ffile& file);
UTILITY_FUNCT_DECL ftime file_time(const ffile& file, ferror& ferr);

UTILITY_FUNCT_DECL bool file_is_writable(const fpath& name);
UTILITY_FUNCT_DECL bool file_is_writable(const fpath& name, ferror& ferr);
     
UTILITY_FUNCT_DECL bool file_is_occupied(const fpath& name);
UTILITY_FUNCT_DECL bool file_is_occupied(const fpath& name, ferror& ferr);
     
UTILITY_FUNCT_DECL bool file_open_with_shell(const fpath& name);
UTILITY_FUNCT_DECL bool file_open_with_shell(const fpath& name, ferror& ferr);

/*
*   file version of windows style
*/
struct fversion
{
    union version_t
    {
        struct 
        {
#ifdef CPU_BIG_ENDIAN
            short major;      //!< ���汾�� 
            short minor;      //!< �ΰ汾��
            short patch;      //!< �޶��汾��
            short build;      //!< �����汾��
#else
            short build;
            short patch;
            short minor;
            short major;
#endif // CPU_BIG_ENDIAN
        };
        int64_t version;    //!< 64λ�汾��
    };

    int       file_flag_mask;  //!< ��: 0x3f 
    int       file_flags;      //!< ��: 0x00
    int       file_os;         //!< ��: VOS_NT_WINDOWS32
    int       file_type;       //!< ��: VFT_APP
    int       file_sub_type;   //!< ��: VFT2_UNKNOWN
    version_t file_version;    //!< �ļ��汾
    version_t product_version; //!< ��Ʒ�汾
};

UTILITY_FUNCT_DECL fversion file_version(const fpath& name);
UTILITY_FUNCT_DECL fversion file_version(const fpath& name, ferror& ferr);

/*
*   ����windows link�ļ�(��ݷ�ʽ)
*   @hot_key ��: (HOTKEYF_CONTROL | HOTKEYF_ALT) << 8 | 0x42 (������� B)
*   �ο�: https://docs.microsoft.com/zh-cn/windows/win32/api/shobjidl_core/nf-shobjidl_core-ishelllinka-gethotkey
*   @show_cmd
*   �ο�: https://docs.microsoft.com/zh-cn/windows/win32/api/winuser/nf-winuser-showwindow
*/
UTILITY_FUNCT_DECL void link_create(
    const util::fpath& file_path,
    const util::fpath& link_path,
    const util::fpath& icon_path = L"",
    const util::fpath& run_cmd   = L"",
    const short        hot_key   = 0,
    const int          show_flag = 0);
UTILITY_FUNCT_DECL void link_create(
    const util::fpath& file_path,
    const util::fpath& link_path,
    const util::fpath& icon_path,
    const util::fpath& run_cmd,
    const short        hot_key,
    const int          show_flag,
    ferror           & ferr);

UTILITY_FUNCT_DECL void directories_remove(const fpath& path);
UTILITY_FUNCT_DECL void directories_remove(const fpath& path, ferror& ferr);

UTILITY_FUNCT_DECL void directories_create(const fpath& path);
UTILITY_FUNCT_DECL void directories_create(const fpath& path, ferror& ferr);

UTILITY_FUNCT_DECL void directory_authorization(const fpath& path);
UTILITY_FUNCT_DECL void directory_authorization(const fpath& path, ferror& ferr);

} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/file_util.ipp"
#endif

#endif // file_util_h__
