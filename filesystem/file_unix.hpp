#ifndef file_unix_h__
#define file_unix_h__

/*
*   file_unix.hpp 
*
*   v0.1 2020-12 by GuoJH
* 
*/

#ifndef file_util_h__
#   include <filesystem/file_util.hpp>
#endif

namespace util{
namespace posix{

} // posix
} // util

#ifndef UTILITY_DISABLE_HEADONLY
#   include "impl/file_unix.ipp"
#endif

#endif // file_unix_h__
