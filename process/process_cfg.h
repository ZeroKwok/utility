#ifndef process_cfg_h__
#define process_cfg_h__

#include <utility.hpp>

#ifdef UTILITY_ENABLE_AUTO_LINK
#   ifndef LINK_PROCESS
#      define LINK_PROCESS
#      define AUTO_LINK_LIB_NAME process
#      include <link.hpp>
#   endif
#endif

#endif // process_cfg_h__
