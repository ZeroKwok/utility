//
// _prefix.hpp
// ~~~~~~~~~~~~~~

// No include guard.

// Disable some pesky MSVC warnings.
#if defined(_MSC_VER)
# pragma warning (push)
# pragma warning (disable:4996)
# pragma warning (disable:4267)
#endif // defined(_MSC_VER)

// Force external visibility of all types.
#if defined(__GNUC__)
# if (__GNUC__ == 4 && __GNUC_MINOR__ >= 1) || (__GNUC__ > 4)
#  pragma GCC visibility push (default)
# endif // (__GNUC__ == 4 && __GNUC_MINOR__ >= 1) || (__GNUC__ > 4)
#endif // defined(__GNUC__)
