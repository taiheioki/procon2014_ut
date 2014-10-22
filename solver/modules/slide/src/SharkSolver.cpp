#include <algorithm>
#include <functional>
#include <limits>
#include <numeric>

#include <boost/format.hpp>

#include "OctopusBoard.hpp"
#include "FixBoard.hpp"
#include "branch.hpp"
#include "Point.hpp"
#include "SharkSolver.hpp"

namespace slide
{

bool SharkSolver::verbose = false;

template<int H, int W>
void SharkSolver::greedy(const PlayBoard<H, W>& board)
{
    using SharkBoard = SharkUrchinBoard<H, W>;
    //using SharkBoard = SharkOctopusBoard<H, W>;

    int offset = 0;
    int bestScore = std::numeric_limits<int>::max();
    SharkBoard returnAnswerBoard;

    reps(i, offset, board.height() - offset) reps(j, offset, board.width() - offset){
        SharkBoard sharkBoard(board);
        sharkBoard.select(sharkBoard.find(Point(i, j).toInt()));

        for(;;){
            sharkBoard.getCandidates(cand);
            verbose && std::cerr << sharkBoard << std::endl;
            verbose && std::cerr << sharkBoard.fixed << std::endl;
            if(cand.empty()){
                break;
            }

            double minScore = std::numeric_limits<double>::max();
            // std::partial_sort(cand.begin(), cand.begin() + 2, [](const Point& a, const Point& b){
            //     const double score_a = sharkBoard.evaluate(a);
            //     const double score_b = sharkBoard.evaluate(b);
            //     return score_a < score_b;
            // });
            Point bestPoint = cand[0];
            Point secondBestPoint = ( static_cast<int>(cand.size()) >= 2 ? cand[1] : cand[0]);

            for(Point p : cand){
                const double score = sharkBoard.evaluate(p);
                if(score < minScore){
                    minScore = score;
                    secondBestPoint = bestPoint;
                    bestPoint = p;
                }
            }

            if(!sharkBoard.align(bestPoint)) {
                if(!sharkBoard.align(secondBestPoint)){
                    verbose && std::cerr << "align failed" << std::endl;
                    verbose && std::cerr << sharkBoard << std::endl;
                    break;
                }
            }
        }

        sharkBoard.answer.optimize();
        if(bestScore > sharkBoard.answer.size()){
            bestScore = sharkBoard.answer.size();
            returnAnswerBoard = sharkBoard;
        }
        
    }

    verbose && std::cout << "\n";
    verbose && std::cout << returnAnswerBoard << std::endl;
    onCreatedAnswer(returnAnswerBoard.answer); 
}


template<int H, int W>
void SharkSolver::solve(const PlayBoard<H, W>& board)
{
    SharkOctopusBoard<H, W> sharkBoard(board);
    //SharkUrchinBoard<H, W> sharkBoard(board);
	Point selectPoint(sharkBoard.height() / 2, sharkBoard.width() / 2);
	sharkBoard.select(sharkBoard.find(sharkBoard.correctId(selectPoint)));

    for(;;){
		sharkBoard.getCandidates(cand);

        if(cand.empty())
            break;

        double minScore = std::numeric_limits<double>::max();
        Point bestPoint;
        for(Point p : cand){
            const double score = sharkBoard.evaluate(p);
            if(score < minScore){
                minScore = score;
                bestPoint = p;
            }
        }

        bool isSuccess = sharkBoard.align(bestPoint);
        if(!isSuccess) std::cerr << "align failed" << std::endl;
	}

	sharkBoard.answer.optimize();
	onCreatedAnswer(sharkBoard.answer);
}

template<>
void SharkSolver::solve<Flexible, Flexible>(const PlayBoard<Flexible>& board)
{
    branch(make_solve_caller(*this), board);
}

void SharkSolver::solve()
{
    greedy(PlayBoard<Flexible>(problem.board));
    //solve(PlayBoard<Flexible>(problem.board));
}

} // end of namespace slide
