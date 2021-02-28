#ifndef filesystem_cfg_h__
#define filesystem_cfg_h__

#include <utility.hpp>

#ifdef UTILITY_ENABLE_AUTO_LINK
#   ifndef LINK_FILESYSTEM
#      define LINK_FILESYSTEM
#      define AUTO_LINK_LIB_NAME filesystem
#      include <link.hpp>
#   endif
#endif

#endif // filesystem_cfg_h__
