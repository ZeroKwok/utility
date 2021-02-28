#ifndef simple_lock_h__
#define simple_lock_h__

/*
*   simple_lock.hpp 
*
*   2018-03 By GuoJH
*/

#include <windows.h>

namespace util {

class simple_lock  
{  
    CRITICAL_SECTION __cs;
public:  
    simple_lock()  
    {  
        // The second parameter is the spin count, for short-held locks it avoid the
        // contending thread from going to sleep which helps performance greatly.
        ::InitializeCriticalSectionAndSpinCount(&__cs, 2000);
    } 

    ~simple_lock()  
    {  
        ::DeleteCriticalSection(&__cs);  
    }  

    // If the lock is not held, take it and return true.  If the lock is already
    // held by something else, immediately return false.
    bool try_lock()
    {
        if (::TryEnterCriticalSection(&__cs) != FALSE)
            return true;

        return false;
    }

    // Take the lock, blocking until it is available if necessary.
    void lock()  
    {  
        ::EnterCriticalSection(&__cs);  
    }  

    // Release the lock.  This must only be called by the lock's holder: after
    // a successful call to Try, or a call to Lock.
    void unlock()  
    {  
        ::LeaveCriticalSection(&__cs);  
    }  
};

// A helper class that acquires the given Lock while the auto_lock is in scope.
class auto_lock  
{  
    auto_lock(const auto_lock&);
    auto_lock& operator=(const auto_lock&);

    simple_lock& __pLock;
public:  
    explicit auto_lock(simple_lock& pLock)
        : __pLock(pLock)
    {   
        __pLock.lock();
    }  

    ~auto_lock()  
    {  
        __pLock.unlock();  
    }  
};

} // util

#endif // simple_lock_h__