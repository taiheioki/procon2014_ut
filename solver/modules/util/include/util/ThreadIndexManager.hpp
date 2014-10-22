#ifndef UTIL_THREAD_INDEX_MANAGER_HPP_
#define UTIL_THREAD_INDEX_MANAGER_HPP_

#include <mutex>

#include <tbb/enumerable_thread_specific.h>

#include "util/define.hpp"
#include "util/SpinMutex.hpp"

namespace util
{

class ThreadIndexManager
{
private:
    using Storage = tbb::enumerable_thread_specific<uchar, tbb::cache_aligned_allocator<uchar>, tbb::ets_key_per_instance>;

    static Storage storage;
    static int size;
    static SpinMutex mutex;

    ThreadIndexManager() = delete;
    ThreadIndexManager(const ThreadIndexManager&) = delete;

public:
    static int getLocalId()
    {
        bool exist;
        uchar& ref = storage.local(exist);
        if(!exist){
            std::lock_guard<SpinMutex> lock(mutex);
            ref = size;
            ++size;
        }
        return ref;
    }

    static void clear()
    {
        storage.clear();
        size = 0;
    }
};

} // end of namespace util

#endif
