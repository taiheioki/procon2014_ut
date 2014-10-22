#ifndef SLIDE_ANSWER_LINEAR_TREE_HPP_
#define SLIDE_ANSWER_LINEAR_TREE_HPP_

#include <mutex>
#include <vector>

#include "Answer.hpp"
#include "util/SpinMutex.hpp"

namespace slide
{

class AnswerLinearTreeNode
{
public:
    uint parentIndex;
    Move move;
    uchar parentThread;

    AnswerLinearTreeNode() = default;
    AnswerLinearTreeNode(uint parentIndex, Move move, uchar parentThread)
        : parentIndex(parentIndex), move(move), parentThread(parentThread) {}
};

class AnswerLinearTree
{
private:
    AnswerLinearTree() = delete;

public:
    static std::vector<std::vector<AnswerLinearTreeNode>> nodes;
    static util::SpinMutex mutex;

    static uint add(uint parentIndex, Move move, uchar parentThread, uchar thisThread)
    {
        if(nodes.size() <= thisThread){
            std::lock_guard<util::SpinMutex> lock(mutex);
            if(nodes.size() <= thisThread){
                nodes.resize(thisThread + 1);
            }
        }

        const uint ret = nodes[thisThread].size();
        nodes[thisThread].emplace_back(parentIndex, move, parentThread);
        return ret;
    }
};

} // end of namespace slide

#endif
