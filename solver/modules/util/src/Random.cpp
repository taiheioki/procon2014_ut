#include <tbb/task_scheduler_init.h>
#include "Random.hpp"

namespace util
{

std::vector<Random> Random::random(tbb::task_scheduler_init::default_num_threads());
util::SpinMutex Random::mutex;

} // end of namespace util
