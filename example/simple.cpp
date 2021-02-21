#include <filesystem/file_util.hpp>
#include <filesystem/path_util.hpp>

#if OS_POSIX
static const char exist_root[]                  = R"(/)";
static const char exist_directory[]             = R"(/etc)";
static const char exist_file[]                  = R"(/etc/passwd)";
#else
static const char exist_root[]                  = R"(C:\)";
static const char exist_directory[]             = R"(C:\Windows)";
static const char exist_file[]                  = R"(C:\Windows\notepad.exe)";
#endif

static const char  current_directory_temp_file [] = "hello1.tmp";
static const char  current_directory_temp_file2[] = "hello2.tmp";

static std::string gloabl_buffer                  = "Hello Utility!";

int main()
{
    util::ferror ferr;
    util::file_exist(exist_root, ferr);
    util::file_exist(exist_file, ferr);
}












