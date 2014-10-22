#include "ThreadIndexManager.hpp"

namespace util
{

ThreadIndexManager::Storage ThreadIndexManager::storage;
int ThreadIndexManager::size(0);
SpinMutex ThreadIndexManager::mutex;

} // end of namespace util
