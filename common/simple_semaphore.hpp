#ifndef simple_semaphore_h__
#define simple_semaphore_h__

#include <mutex>
#include <condition_variable>

class semaphore 
{
public:
    semaphore(int value=1)
        : count{value}
    {}
    
    void wait()
    {
        std::unique_lock<std::mutex> lock{mutex};
        if (--count<0) // count is not enough ?
            condition.wait(lock); // suspend and wait...
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
    
#endif // simple_semaphore_h__
