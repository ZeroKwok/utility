#ifdef UTILITY_DISABLE_HEADONLY
#   include "../file_util.hpp"
#endif

namespace util {

ffile::ffile(native_type fid)
    : _native_id(fid)
    , _open_flags(0)
{}

ffile::ffile(native_type fid, open_flags flags)
    : _native_id(fid)
    , _open_flags(flags)
{}

ffile::ffile(ffile& other)
    : _native_id(other._native_id)
    , _open_flags(other._open_flags)
{
    other._native_id = -1;
}

ffile::~ffile()
{
    close();
}

ffile& ffile::operator=(ffile& right)
{
    close();

    _native_id  = right._native_id;
    _open_flags = right._open_flags;

    right._native_id = -1;
    right._open_flags = 0;

    return *this;
}

#if __cplusplus >= 201103L || _MSVC_LANG >= 201103L

ffile::ffile(ffile&& other)
    : _native_id(other._native_id)
    , _open_flags(other._open_flags)
{
    other._native_id = -1;
}

ffile& ffile::operator=(ffile&& right)
{
    close();

    _native_id  = right._native_id;
    _open_flags = right._open_flags;

    right._native_id = -1;
    right._open_flags = 0;

    return *this;
}

#endif // C++ 11

bool ffile::vaild() const
{
    return _native_id != -1 &&
           _native_id != 0;
}

ffile::open_flags ffile::flags() const
{
    return _open_flags;
}

ffile::native_type ffile::native_id() const
{
    return _native_id;
}

ffile::operator bool() const
{
    return vaild();
}

ffile::operator native_type() const
{
    return native_id();
}

} // util