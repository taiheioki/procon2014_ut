#include <tbb/task_scheduler_init.h>
#include "AnswerLinearTree.hpp"

namespace slide
{

std::vector<std::vector<AnswerLinearTreeNode>> AnswerLinearTree::nodes(tbb::task_scheduler_init::default_num_threads());
util::SpinMutex AnswerLinearTree::mutex;

} // end of namespace slide
