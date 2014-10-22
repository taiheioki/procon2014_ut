#include <algorithm>
#include <thread>
#include <vector>

#include <tbb/parallel_sort.h>
#include <tbb/task_scheduler_init.h>

#include "KurageBoard.hpp"
#include "KurageSolver.hpp"

#include "util/dense_hash_map.hpp"
#include "util/Random.hpp"
#include "util/SpinMutex.hpp"
#include "util/StopWatch.hpp"

namespace slide
{

bool KurageSolver::verbose = false;

void KurageSolver::solve()
{
    return solve(PlayBoard<Flexible>(problem.board), problem.selectionLimit);
}

} // end of namespace slide
