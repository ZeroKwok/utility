#ifndef string_cfg_h__
#define string_cfg_h__

#include <utility.hpp>

#ifdef UTILITY_ENABLE_AUTO_LINK
#   ifndef LINK_STRING
#      define LINK_STRING
#      define AUTO_LINK_LIB_NAME string
#      include <link.hpp>
#   endif
#endif

#endif // string_cfg_h__
