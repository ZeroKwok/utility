#ifndef string_cfg_h__
#define string_cfg_h__

#include <utility.hpp>

#ifdef UTILITY_ENABLE_AUTO_LINK
#   ifndef LINK_PLATFORM
#      define LINK_PLATFORM
#      define AUTO_LINK_LIB_NAME process
#      include <link.hpp>
#   endif
#endif

#endif // string_cfg_h__
