#ifndef platform_cfg_h__
#define platform_cfg_h__

#include <utility.hpp>

#ifdef UTILITY_ENABLE_AUTO_LINK
#   ifndef LINK_PLATFORM
#      define LINK_PLATFORM
#      define AUTO_LINK_LIB_NAME platform
#      include <link.hpp>
#   endif
#endif

#endif // platform_cfg_h__
