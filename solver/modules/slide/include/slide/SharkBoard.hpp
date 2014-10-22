#ifndef SLIDE_SHARK_BOARD_HPP_
#define SLIDE_SHARK_BOARD_HPP_

#include <cstdlib>

#include "FixBoard.hpp"
#include "OctopusBoard.hpp"
#include "UrchinBoard.hpp"


namespace slide
{

template<typename Base>
class SharkBoard : public Base
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

	// from PlayBoard
	using Base::selected;

	// from FixBoard
	using Base::fixed;

	double ManhattanDistanceCoefficient;
	double HeightAndWidthCoefficient;
	double MeanCoefficient;
	double MoveCountCoefficient;

	SharkBoard() = default;

	SharkBoard(const PlayBoardBase<H, W>& board) {
		init(board);
	}
	
	void init(const PlayBoardBase<H, W>& board)
	{
		Base::init(board);
		ManhattanDistanceCoefficient = 1.0;
		HeightAndWidthCoefficient = 12.0;
		MeanCoefficient = 1.0;
		MoveCountCoefficient = ManhattanDistanceCoefficient;
	}

	double evaluate(Point candidatePoint)
	{
		double s = 0.0;
		SharkBoard tmp = *this;
		tmp.align(candidatePoint);

		s += MeanCoefficient * evaluateMean(tmp);
		s += tmp.answer.size();
  		s +=  2.0 * manhattan(tmp);
  		//s +=  1.0 * linearConflict(candidatePoint);
		return s;
	}

	double ManhattanDistance(Point candidate) const
	{
		Point target = find(correctId(candidate));
		double dist = (target - selected).l1norm();
		return dist;
	}

	double evaluateMoveCount(Point candidate) const
	{
		Point target = find(correctId(candidate));
  		int tmp_x = std::abs(candidate.x - target.x);
		int tmp_y = std::abs(candidate.y - target.y);
		return std::min(tmp_x, tmp_y) + 5.0 / 6.0 * std::abs(tmp_x - tmp_y);
	}

	int evaluateHeightAndWidth(Point candidate)
	{
		int y, x, Height, Width;
		fixed.set(candidate);
		fixed.largetstRect(y, x, Height, Width);
		fixed.reset(candidate);
		return Height * Width;
	}

	double evaluateMean(Point candidate)
	{
		int x_sum = 0;
		int y_sum = 0;
		int x_median = Point(at(selected)).x;
		int y_median = Point(at(selected)).y;
		int cnt = 1;

		fixed.set(candidate);
		rep(y, height()){
			rep(x, width()){
				if(fixed(y, x)) continue;
				x_sum += std::abs(x - x_median);
				y_sum += std::abs(y - y_median);
				cnt++;
			}
		}
		fixed.reset(candidate);

		return x_sum + y_sum;
	}

	double evaluateMean(SharkBoard board) const 
	{
		int x_sum = 0;
		int y_sum = 0;
		int x_median = Point(board.at(selected)).x;
		int y_median = Point(board.at(selected)).y;
		int cnt = 1;

		rep(y, height()){
			rep(x, width()){
				if(board.fixed(y, x)) continue;
				x_sum += std::abs(x - x_median);
				y_sum += std::abs(y - y_median);
				cnt++;
			}
		}

		return x_sum + y_sum;
	}


	int linearConflict(Point p) const
    {
        int sum = 0;

        // 横
        rep(i, height()) reps(j, 1, width()){
        	if(Point(i, j) == p){
        		continue;
        	}
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

    int manhattan(SharkBoard board) const 
    {
    	int selManhattan = 0;
    	int manhattan = 0;

        rep(i, height()) rep(j, width()){
            if(Point(i, j) == board.selected){
            	Point selDestination = board.find(board.at(i, j));
                selManhattan = (Point(i, j) - selDestination).l1norm();
            }
            else{
               	manhattan += (Point(i, j) - Point(board.at(i, j))).l1norm();
            }
        }
        return manhattan + 2 * selManhattan;
    }

};

template<int H, int W = H>
using SharkOctopusBoard = SharkBoard<OctopusBoard<FixAnswerBoard<H, W>>>;

template<int H, int W = H>
using SharkUrchinBoard = SharkBoard<UrchinBoard<FixAnswerBoard<H, W>>>;

} // end of namespace slide

#endif