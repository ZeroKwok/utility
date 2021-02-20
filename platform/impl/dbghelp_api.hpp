#ifndef dbghelp_api_h__
#define dbghelp_api_h__

#include <dbghelp.h>

namespace winapi
{
    enum InitState
    {
        InitNone,
        InitSusseed,
        InitFailed,
    };

    static int      bInitState = InitNone;
    static HMODULE  hDbghelpMoudle = nullptr;

    // DbgHelp.dll 5.1 or later
    decltype(SymFromAddrW)                   * fnSymFromAddrW = nullptr;
    decltype(SymInitializeW)               * fnSymInitializeW = nullptr;
    decltype(SymGetModuleInfoW64)     * fnSymGetModuleInfoW64 = nullptr;
    decltype(SymGetLineFromAddrW64) * fnSymGetLineFromAddrW64 = nullptr;

#ifndef __dbghelp_api_load_function
#   define __dbghelp_api_load_function(h, name) \
        do{ \
            fn##name = (decltype(fn##name))GetProcAddress(h, #name); \
            if (fn##name == nullptr) \
                return false; \
        } while (0)
#endif

    inline bool init_dbghelp_api()
    {
        if (bInitState == InitNone)
        {
            bInitState = InitFailed;

            hDbghelpMoudle = ::LoadLibraryA("Dbghelp.dll");
            if (hDbghelpMoudle == nullptr)
                return false;

            //fnSymFromAddrW = (decltype(fnSymFromAddrW))GetProcAddress(hDbghelpMoudle, "SymFromAddrW");

            __dbghelp_api_load_function(hDbghelpMoudle, SymFromAddrW);
            __dbghelp_api_load_function(hDbghelpMoudle, SymInitializeW);
            __dbghelp_api_load_function(hDbghelpMoudle, SymGetModuleInfoW64);
            __dbghelp_api_load_function(hDbghelpMoudle, SymGetLineFromAddrW64);

            bInitState = InitSusseed;
            return true;
        }

        return bInitState == InitSusseed;
    }

    inline void uninit_dbghelp_api()
    {
        if (bInitState == InitSusseed)
        {
            if(hDbghelpMoudle)
                ::FreeLibrary(hDbghelpMoudle);

            hDbghelpMoudle = nullptr;
            fnSymFromAddrW = nullptr;
            fnSymInitializeW = nullptr;
            fnSymGetModuleInfoW64 = nullptr;
            fnSymGetLineFromAddrW64 = nullptr;
        }
    }

}

#endif // dbghelp_api_h__
