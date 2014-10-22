#ifndef SLIDE_ANSWER_SHARED_TREE_HPP_
#define SLIDE_ANSWER_SHARED_TREE_HPP_

#include <algorithm>
#include <memory>
#include <utility>

#include "Answer.hpp"

namespace slide
{

class AnswerSharedTreeNode
{
public:
    std::shared_ptr<AnswerSharedTreeNode> parent;
    Move move;

    AnswerSharedTreeNode() = default;
    AnswerSharedTreeNode(const std::shared_ptr<AnswerSharedTreeNode>& parent, Move move) : parent(parent), move(move) {}

    void release()
    {
        while(parent){
            if(!parent.unique()){
                parent.reset();
                break;
            }
            parent = std::move(parent->parent);
        }
    }
};

} // end of namespace slide

#endif
