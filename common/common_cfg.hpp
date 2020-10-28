#ifndef common_cfg_h__
#define common_cfg_h__

#include <utility.hpp>

#ifdef UTILITY_ENABLE_AUTO_LINK
#   ifndef LINK_COMMON
#      define LINK_COMMON
#      define AUTO_LINK_LIB_NAME common
#      include <link.hpp>
#   endif
#endif

#endif // common_cfg_h__
