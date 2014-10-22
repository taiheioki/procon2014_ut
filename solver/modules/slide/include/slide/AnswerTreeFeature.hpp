#ifndef ANSWER_TREE_FEATURE_HPP_
#define ANSWER_TREE_FEATURE_HPP_

#include <algorithm>
#include <limits>

#include "AnswerLinearTree.hpp"
#include "util/ThreadIndexManager.hpp"

namespace slide
{

class AnswerTreeFeature
{
public:
    uint index;
    uchar thread;

    AnswerTreeFeature()
    {
        init();
    }

    void init()
    {
        index = std::numeric_limits<uint>::max();
        thread = 0;
    }

    void move(Direction dir, uchar thisThread = util::ThreadIndexManager::getLocalId())
    {
        index = AnswerLinearTree::add(index, Move(dir), thread, thisThread);
        thread = thisThread;
    }

    void select(Point newSelect, uchar thisThread = util::ThreadIndexManager::getLocalId())
    {
        index = AnswerLinearTree::add(index, Move(newSelect), thread, thisThread);
        thread = thisThread;
    }

    Answer buildAnswer() const
    {
        Answer answer;

        uint nowIndex = index;
        uchar nowThread = thread;

        while(nowIndex != std::numeric_limits<uint>::max()){
            const AnswerLinearTreeNode& p = AnswerLinearTree::nodes[nowThread][nowIndex];
            answer.push_back(p.move);
            nowIndex = p.parentIndex;
            nowThread = p.parentThread;
        }

        std::reverse(answer.begin(), answer.end());
        return answer;
    }
};

} // end of namespace slide

#endif
