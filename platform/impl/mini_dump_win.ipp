#ifdef UTILITY_DISABLE_HEADONLY
#   include "../mini_dump_win.h"
#endif

#include <ctime>
#include <windows.h>
#include <DbgHelp.h>
#include <common/time_util.h>
#include <string/string_util.h>
#include <filesystem/path_util.h>

#pragma comment(lib, "Dbghelp.lib")

namespace util {
namespace win {

struct mini_dump_private
{
    static void crash_hander()
    {
        mini_dump::instance().crash_hander();
    }
};

namespace detail {

// mini_dump 模块信息过滤回调, 用户筛选模块中的那些信息要写入dump.
inline BOOL CALLBACK MiniDumpCallback(
    _Inout_ PVOID CallbackParam,
    _In_    PMINIDUMP_CALLBACK_INPUT CallbackInput,
    _Inout_ PMINIDUMP_CALLBACK_OUTPUT CallbackOutput)
{
    if(CallbackInput == NULL || CallbackOutput == NULL)
        return FALSE;

    BOOL retsult = FALSE;
    switch(CallbackInput->CallbackType)
    {
    case IncludeModuleCallback: // 是否包含特定的模块信息
    case IncludeThreadCallback:
    case ThreadCallback:
    case ThreadExCallback:
        retsult = TRUE;         // 返回true, 将指定的信息输出到minidump文件
        break;

    case ModuleCallback:        // 指定模块的那些信息包含在模块中
    {
        // ModuleReferencedByMemory 指示通过线程的堆栈指针,或后备存储引用的模块,
        // 这个值仅在MiniDumpWriteDump函数的dumpType指定了MiniDumpScanMemory方才有效.

        if (!(CallbackOutput->ModuleWriteFlags & ModuleReferencedByMemory))
            CallbackOutput->ModuleWriteFlags &= ~ModuleWriteModule; // 去除仅写模块信息标志位

        retsult = TRUE;
    }
        break;

    default:
        break;
    }

    return retsult;
}

// 创建mini dump
inline DWORD CreateMiniDump(
    const std::wstring& file,  
    EXCEPTION_POINTERS* excptionPtr = nullptr)
{
    HANDLE hFile = ::CreateFileW(
        file.c_str(), 
        GENERIC_WRITE | GENERIC_READ, 
        FILE_SHARE_WRITE | FILE_SHARE_READ, 
        NULL, 
        CREATE_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        return GetLastError();

    // 描述导致生成minidump的客户机异常, 如果该参数的值为NULL，则在小文件中不包含异常信息;
    MINIDUMP_EXCEPTION_INFORMATION exceptionParam = {
        ::GetCurrentThreadId(),
        excptionPtr,
        0,
    };

    MINIDUMP_CALLBACK_INFORMATION mci = {
        (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback,
        0,
    };

    // 转储信息的类型
    // MiniDumpWithIndirectlyReferencedMemory 包括具有本地变量或其他堆栈内存引用的数据的页面, 此选项可以显著增加文件大小。
    // MiniDumpScanMemory 与MiniDumpCallback共同使用来确定是否将某一特定模块以及该模块的信息写入到所生成的DMP文件中。
    // 
    // https://docs.microsoft.com/en-us/windows/win32/api/minidumpapiset/ne-minidumpapiset-minidump_type

    MINIDUMP_TYPE dumpType = (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory); 

    // 进程句柄必须具有PROCESS_QUERY_INFORMATION, PROCESS_VM_READ权限, PROCESS_DUP_HANDLE是可选的用于收集某些信息.
    if (!::MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        dumpType,
        excptionPtr != NULL ? &exceptionParam : NULL,
        NULL,
        &mci))
    {
        return ::GetLastError();
    }

    ::CloseHandle(hFile);

    return S_OK;
}

// 结构化异常过滤器, 用以在崩溃时得到机会处理
inline LONG WINAPI UnhandledExceptionFilter(EXCEPTION_POINTERS* exp)
{
    util::ferror ferr;
    util::fpath  path = util::path_append(
        util::path_from_module_dir(ferr), L"dump");

    util::fpath  name = util::path_find_filename(
        util::path_from_module(ferr));

    if (!util::file_exist(path, ferr))
        util::directories_create(path, ferr);

    path = util::path_append(path, name +
           util::wformat_epoch(::time(0), L".%Y%m%d_%H%M%S.dmp"));
    CreateMiniDump(path, exp);

    mini_dump_private::crash_hander();

    return EXCEPTION_EXECUTE_HANDLER;
}

} // detail

mini_dump::mini_dump()
{}

mini_dump& mini_dump::instance()
{
    static mini_dump __imp;
    return __imp;
}

void mini_dump::create_dump(const std::wstring file)
{
    platform_error err;
    create_dump(file, err);

    if (err)
        throw err;
}

void mini_dump::create_dump(const std::wstring file, platform_error& error)
{
    util::ferror ferr;
    util::fpath path = util::path_find_parent(file);
    if (!util::file_exist(path, ferr))
        util::directories_create(path, ferr);

    if (ferr)
    {
        error = platform_error(ferr.code(), ferr.error_description());
        return;
    }

    DWORD kResult = detail::CreateMiniDump(file, nullptr);

    if (kResult != S_OK)
    {
        error = platform_error(kResult, "Failed to create minimum crash dump");
    }
}

void mini_dump::init()
{
    static bool __inited = false;

    if(!__inited)
    {
        __inited = true;

        // https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-setunhandledexceptionfilter
        // 
        ::SetUnhandledExceptionFilter(detail::UnhandledExceptionFilter);
    }
}

} // win
} // util