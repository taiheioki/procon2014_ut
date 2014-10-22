#ifndef SLIDE_DOLPHIN_BOARD_HPP_
#define SLIDE_DOLPHIN_BOARD_HPP_

#include <cstdlib>

#include "FixBoard.hpp"
#include "OctopusBoard.hpp"
#include "UrchinBoard.hpp"

namespace slide
{

extern double MeanCoefficient;
extern double ManhattanCoefficient;
extern double LinearCoefficient;

template<typename Base>
class DolphinBoard : public Base
{
public:
    // from Board
    using Base::at;
    using Base::correctId;
    using Base::find;
    using Base::height;
    using Base::width;
    using Base::H;
    using Base::W;

    // from AnswerTreeBoard
    using Base::swappingCount;

    // from PlayBoard
    using Base::selected;

    // from FixBoard
    using Base::fixed;

    using Base::Base;

    double evaluate() const
    {
        double score = 0.0;
        score += swappingCount;        
        // score += 350.0 * evaluateMean() / fixed.countZero();
        // score +=  2.0 * manhattan();
        // score +=  1.0 * linearConflict();
        score +=  MeanCoefficient * evaluateMean() / fixed.countZero();
        score +=  ManhattanCoefficient * manhattan();
        score +=  LinearCoefficient * linearConflict();
        //score += 10.0 * util::Random::nextReal();
        return score;
    }

    double align(Point p)
    {
        const bool ret = Base::align(p); 
        if(!ret) return 1 << 29;
        return evaluate();
    }

    int evaluateHeightAndWidth() const
    {
        int y, x, Height, Width;
        fixed.largetstRect(y, x, Height, Width);
        return Height * Width;
    }

    double evaluateMean() const
    {
        int x_sum = 0;
        int y_sum = 0;
        int x_median = (this->width()-1)/2;
        int y_median = (this->height()-1)/2;
        //int x_median = Point(at(selected)).x;
        //int y_median = Point(at(selected)).y;
        int cnt = 1;

        rep(y, height()){
            rep(x, width()){
                if(fixed(y, x)) continue;
                x_sum += std::abs(x - x_median);
                y_sum += std::abs(y - y_median);
                cnt++;
            }
        }
        return x_sum + y_sum;
    }

    int linearConflict() const
    {
        int sum = 0;

        // 横
        rep(i, height()) reps(j, 1, width()){
            if(selected == Point(i, j)){
                continue;
            }

            const Point dst_a = Point(at(i, j));
            if(dst_a.y == i){
                rep(k, j){
                    const Point dst_b = Point(at(i, k));
                    sum += dst_b.y == i && dst_a.x < dst_b.x && selected != Point(i, k);
                }
            }
        }

        // 縦
        rep(i, width()) reps(j, 1, height()){
            if(selected == Point(i, j)){
                continue;
            }

            const Point dst_a = Point(at(j, i));
            if(dst_a.x == i){
                rep(k, j){
                    const Point dst_b = Point(at(k, i));
                    sum += dst_b.x == i && dst_a.y < dst_b.y && selected != Point(k, i);
                }
            }
        }

        return sum;
    }

    int manhattan() const 
    {
        int manhattan = 0;

        rep(i, height()) rep(j, width()){
            if(fixed(i, j)) continue;
            if(Point(i, j) != selected){
                manhattan += (Point(i, j) - Point(at(i, j))).l1norm();
            }
        }
        return manhattan;
    }

};

template<int H, int W = H>
using DolphinOctopusBoard = DolphinBoard<OctopusBoard<FixAnswerTreeBoard<H, W>>>;

template<int H, int W = H>
using DolphinUrchinBoard = DolphinBoard<UrchinBoard<FixAnswerTreeBoard<H, W>>>;

} // end of namespace slide

#endif
