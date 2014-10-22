#ifndef UTIL_SPIN_MUTEX_HPP_
#define UTIL_SPIN_MUTEX_HPP_

#include <atomic>
#include <mutex>

namespace util
{

class SpinMutex
{
private:
    std::atomic_bool locked;

public:
    SpinMutex() : locked(false) {}

    void lock() {
        while(locked.exchange(true, std::memory_order_acquire));
    }

    void unlock() {
        locked.store(false, std::memory_order_release);
    }
};

} // end of namespace util

#endif
