#ifdef UTILITY_DISABLE_HEADONLY
#   include "../mini_dump.hpp"
#endif

#include <windows.h>
#include <DbgHelp.h>
#include <ctime>
#include <common/time_util.hpp>
#include <string/string_util.hpp>
#include <filesystem/path_util.hpp>

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

// mini_dump ģ����Ϣ���˻ص�, �û�ɸѡģ���е���Щ��ϢҪд��dump.
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
    case IncludeModuleCallback: // �Ƿ�����ض���ģ����Ϣ
    case IncludeThreadCallback:
    case ThreadCallback:
    case ThreadExCallback:
        retsult = TRUE;         // ����true, ��ָ������Ϣ�����minidump�ļ�
        break;

    case ModuleCallback:        // ָ��ģ�����Щ��Ϣ������ģ����
    {
        // ModuleReferencedByMemory ָʾͨ���̵߳Ķ�ջָ��,��󱸴洢���õ�ģ��,
        // ���ֵ����MiniDumpWriteDump������dumpTypeָ����MiniDumpScanMemory������Ч.

        if (!(CallbackOutput->ModuleWriteFlags & ModuleReferencedByMemory))
            CallbackOutput->ModuleWriteFlags &= ~ModuleWriteModule; // ȥ����дģ����Ϣ��־λ

        retsult = TRUE;
    }
        break;

    default:
        break;
    }

    return retsult;
}

// ����mini dump
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

    // ������������minidump�Ŀͻ����쳣, ����ò�����ֵΪNULL������С�ļ��в������쳣��Ϣ;
    MINIDUMP_EXCEPTION_INFORMATION exceptionParam = {
        ::GetCurrentThreadId(),
        excptionPtr,
        0,
    };

    MINIDUMP_CALLBACK_INFORMATION mci = {
        (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback,
        0,
    };

    // ת����Ϣ������
    // MiniDumpWithIndirectlyReferencedMemory �������б��ر�����������ջ�ڴ����õ����ݵ�ҳ��, ��ѡ��������������ļ���С��
    // MiniDumpScanMemory ��MiniDumpCallback��ͬʹ����ȷ���Ƿ�ĳһ�ض�ģ���Լ���ģ�����Ϣд�뵽�����ɵ�DMP�ļ��С�
    // 
    // https://docs.microsoft.com/en-us/windows/win32/api/minidumpapiset/ne-minidumpapiset-minidump_type

    MINIDUMP_TYPE dumpType = (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory); 

    // ���̾���������PROCESS_QUERY_INFORMATION, PROCESS_VM_READȨ��, PROCESS_DUP_HANDLE�ǿ�ѡ�������ռ�ĳЩ��Ϣ.
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

// �ṹ���쳣������, �����ڱ���ʱ�õ����ᴦ��
inline LONG WINAPI UnhandledExceptionFilter(EXCEPTION_POINTERS* exp)
{
    util::ferror ferr;
    util::fpath  path = util::path_append(
        util::path_from_module_dir(ferr), L"dump");

    if (!util::file_exist(path, ferr))
        util::directories_create(path, ferr);

    path = util::path_append(path, 
           util::format_epoch(::time(0), "%Y%m%d_%H%M%S.dmp"));
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
        ::SetUnhandledExceptionFilter(detail::UnhandledExceptionFilter);
    }
}

} // win
} // util
