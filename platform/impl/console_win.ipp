/*
*   console_win.ipp
*
*   v0.1 2018-11 By GuoJH
*/

#ifdef UTILITY_DISABLE_HEADONLY
#   include "../console_win.h"
#endif

#include <windows.h>
#include <algorithm>

namespace util {
namespace win  {

void output(
    const std::string& text, 
    const uint32_t handle /*= STD_OUTPUT*/, 
    const uint16_t color  /*= COLOR_DEF*/)
{
    if (is_redirect(handle))
    {
        DWORD lpNumberOfCharsWritten = 0;
        WriteFile(
            ::GetStdHandle(handle), 
            text.data(), (DWORD)text.length(), 
            &lpNumberOfCharsWritten, nullptr);
        return;
    }

    HANDLE hstdout = nullptr;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if(COLOR_DEF != color)
    {
        hstdout = ::GetStdHandle(handle);
        ::GetConsoleScreenBufferInfo(hstdout, &csbi);
        ::SetConsoleTextAttribute(hstdout, color);
    }

    ::WriteConsoleA(::GetStdHandle(handle), text.data(), (DWORD)text.length(), nullptr, nullptr);

    if(hstdout)
        ::SetConsoleTextAttribute(hstdout, csbi.wAttributes);
}

void clear()
{
    ::system("cls");
}

bool is_redirect(const uint32_t handle)
{
    // How do I check if my Delphi console app is redirected to a file or pipe?
    // https://stackoverflow.com/questions/9021916/how-do-i-check-if-my-delphi-console-app-is-redirected-to-a-file-or-pipe

    // https://learn.microsoft.com/zh-cn/windows/console/getstdhandle#return-value
    HANDLE hstdout = ::GetStdHandle(handle);
    if (hstdout == INVALID_HANDLE_VALUE || hstdout == NULL)
        return false;

    // https://learn.microsoft.com/zh-cn/windows/win32/api/fileapi/nf-fileapi-getfiletype
    DWORD type = ::GetFileType(hstdout);
    if (type == FILE_TYPE_DISK || type == FILE_TYPE_PIPE)
        return true;

    // https://learn.microsoft.com/zh-cn/windows/console/getconsolemode
    DWORD mode = 0;
    if (::GetConsoleMode(hstdout, &mode) == 0)
        return true;

    return false;
}

void cursor_goto(const coord& pos)
{
    COORD _pos = { pos.x, pos.y };

    ::SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), _pos);
}

void cursor_goto(short x, short y)
{
    COORD pos = {x, y};
    ::SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

coord cursor_pos()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    ::GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &bInfo);
    
    coord position = {
        bInfo.dwCursorPosition.X,
        bInfo.dwCursorPosition.Y
    };

    return position;
}

void output_progress(
    int progress, const char prefix/* = '>'*/, const char suffix/* = '='*/)
{
    progress = std::max(progress, 0);
    progress = std::min(progress, 100);

    int count = progress / 2;
    for (int i = 0; i < count; ++i) {
        printf("%c", prefix);
    }

    for (int i = 0; i < (50 - count); ++i) {
        printf("%c", suffix);
    }

    printf(" %02d%%", progress);
}

} // console
} // util