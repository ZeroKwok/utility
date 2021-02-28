#ifdef UTILITY_DISABLE_HEADONLY
#   include "../platform_error.h"
#endif

#include <platform/platform_util.h>

namespace util {

platform_error::msgs_type platform_error::error_message() const
{
    return util::format_error(_code);
}

platform_error::msgs_type platform_error::print() const
{ 
    msgs_type msgs = "Platform Error: ";
              msgs += error_description();
              msgs += util::sformat("\r\nError details: 0x08x, ", code());
              msgs += error_message();
    return msgs;
}

} // util