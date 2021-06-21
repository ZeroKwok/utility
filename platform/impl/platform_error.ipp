#ifdef UTILITY_DISABLE_HEADONLY
#   include "../platform_error.h"
#endif

#include <string/tstring.h>
#include <string/string_util.h>
#include <platform/platform_util.h>

namespace util {

platform_error::msgs_type platform_error::error_message() const
{
    return util::format_error(_code);
}

platform_error::msgs_type platform_error::print() const
{ 
    msgs_type msgs = util::tstring("Platform Error: {1}(Error details: {2},{3})")
        % error_description()
        % util::sformat("0x%08x", code())
        % error_message();
    return msgs;
}

} // util