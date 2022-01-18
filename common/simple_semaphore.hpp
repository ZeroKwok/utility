#ifndef simple_semaphore_h__
#define simple_semaphore_h__

#include <mutex>
#include <condition_variable>

namespace util {

class semaphore 
{
public:
    semaphore(int value=1)
        : count{value}
    {}
    
    bool wait(int millisecond = -1)
    {
        std::unique_lock<std::mutex> lock{mutex};
        if (--count < 0) // count is not enough ?
        {
            // suspend and wait...
            if (millisecond < 0) 
                condition.wait(lock);
            else
            {
                return condition.wait_for(lock,
                    std::chrono::milliseconds(millisecond))
                    == std::cv_status::no_timeout;
            }
        }

        return true;
    }
    
    void signal()
    {
        std::lock_guard<std::mutex> lock{mutex};
        if(++count<=0) // have some thread suspended ?
            condition.notify_one(); // notify one !
    }
    
private:
    int count;
    std::mutex mutex;
    std::condition_variable condition;
};

} // namespace util

#endif // simple_semaphore_h__
