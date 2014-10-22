#include <algorithm>
#include <functional>
#include <limits>

#include <boost/format.hpp>

#include "StraightSolver.hpp"

namespace slide
{

void StraightSolver::alignRow(AnswerBoard<Flexible>& board)
{
	BOOST_ASSERT(board.height() >= 3 && board.width() >= 2);

	// 右端2個以外
	rep(dstX, board.width()-2){
		const Point dst(board.height()-1, dstX);
		const Point src = board.find(board.correctId(dst));

		// 既に大丈夫だった
		if(src == dst){
			continue;
		}

		if(dst.x == src.x){
			if(dst.x <= board.selected.x){
				moveTargetPiece(board, src, dst, 0, dst.x, board.height(), board.width()-dst.x);
			}
			else{
				moveTargetPiece(board, src, dst + Point(-1, 0), 0, 0, board.height()-1, board.width());
				board.moveVertically(dst.y - 2);
				board.moveHorizontally(dst.x);
				board.moveRight();
				board.moveDown();
				board.moveDown();
				board.moveLeft();
				board.moveUp();
			}
		}
		else if(dst.x < src.x){
			if(board.selected.x < dst.x){
				board.moveHorizontally(dst.x);
			}
			moveTargetPiece(board, src, dst, 0, dst.x, board.height(), board.width()-dst.x);
		}
		else{
			if(dst.x < board.selected.x){
				board.moveHorizontally(dst.x);
			}
			if(board.selected.y == dst.y){
				board.moveUp();
			}

			moveTargetPiece(board, src, dst + Point(-1, 0), 0, 0, board.height(), dst.x+1);

			// src と dst は絶対に同じにならない
			if(board.selected.x == dst.x - 1){
				board.moveUp();
				board.moveRight();
			}
			board.moveRight();
			board.moveDown();
			board.moveDown();
			board.moveLeft();
			board.moveUp();
		}
	}

	if(board.isAligned(board.height()-1, board.width()-1)){
		const uchar pieceId = board.correctId(board.height()-1, board.width()-2);
		if(board(board.height()-1, board.width()-2) == pieceId){
			return;
		}
		else if(board(board.height()-2, board.width()-2) == pieceId && board.selected == Point(board.height()-1, board.width()-2)){
			board.moveUp();
			return;
		}
	}

	// 最後の二つ目
	{
		const uchar pieceId = board.correctId(board.height()-1, board.width()-2);
		const Point src = board.find(pieceId);
		const Point dst(board.height()-1, board.width()-1);

		if(src == dst);
		else if(src.y == dst.y){
			board.moveHorizontally(board.width() - 1);
			board.moveVertically(board.height() - 1);
			board.moveLeft();
		}
		else if(board.selected.y == dst.y && board.width()-2 <= src.x){
			moveTargetPiece(board, src, dst, 0, board.width()-2, board.height(), 2);
		}
		else{
			if(board.selected.y == dst.y){
				board.moveUp();
			}

			moveTargetPiece(board, src, Point(dst.y-1, dst.x), 0, 0, board.height()-1, board.width());
			board.moveHorizontally(board.width() - 2);
			board.moveVertically(board.height() - 1);
			board.moveRight();
			board.moveUp();
		}
	}

	// ラス1
	{
		const uchar pieceId = board.correctId(board.height()-1, board.width()-1);
		if(board.selected.y == board.height()-1 && board(board.height()-2, board.width()-2) != pieceId){
			board.moveUp();
		}

		if(board.selected.y != board.height()-1 && board(board.height()-1, board.width()-2) != pieceId){

			const Point src = board.find(pieceId);
			const Point dst(board.height()-2, board.width()-1);
			moveTargetPiece(board, src, dst, 0, 0, board.height()-1, board.width());

			board.moveHorizontally(board.width() - 2);
			board.moveVertically(board.height() - 1);
			board.moveRight();
			board.moveUp();
		}
		else if(board.width() >= 3){
			board.moveHorizontally(board.width() - 2);
			board.moveVertically(board.height() - 1);
			board.moveRight();
			board.moveUp();
			board.moveLeft();
			board.moveLeft();
			board.moveDown();
			board.moveRight();
			board.moveRight();
			board.moveUp();
			board.moveLeft();
			board.moveDown();
			board.moveLeft();
			board.moveUp();
		}
		else{
			board.moveHorizontally(0);
			board.moveVertically(board.height() - 1);
			board.moveRight();
			board.moveUp();
			board.moveLeft();
			board.moveUp();
			board.moveRight();
			board.moveDown();
			board.moveDown();
			board.moveLeft();
			board.moveUp();
			board.moveRight();
			board.moveUp();
			board.moveLeft();
			board.moveDown();
			board.moveDown();
			board.moveRight();
			board.moveUp();
		}
	}
}

void StraightSolver::moveTargetPiece(AnswerBoard<Flexible>& board, Point src, Point dst, int y, int x, int height, int width)
{
	BOOST_ASSERT(src.isIn(y, x, height, width));
	BOOST_ASSERT(dst.isIn(y, x, height, width));
	BOOST_ASSERT(board.selected.isIn(y, x, height, width));
	BOOST_ASSERT(0 <= y && y+height <= board.height() && 0 <= x && x+width <= board.width());
	BOOST_ASSERT(height >= 2 && width >= 2);
	BOOST_ASSERT(src != board.selected);

	// 既に大丈夫だった
	if(src == dst){
		return;
	}

	const uchar pieceId = board(src);
	const Point sel = board.selected;

	// src に隣接する有効なセルを列挙
	Direction dir = Direction::Up;
	Point adj = Point(-1, -1);
	int minDist = std::numeric_limits<int>::max();

	// for each directions
	rep(k, 4){
		const Direction nowdir = Direction(k);
		if(dst.y >= src.y && nowdir == Direction::Up)    continue;
		if(dst.y <= src.y && nowdir == Direction::Down)  continue;
		if(dst.x >= src.x && nowdir == Direction::Left)  continue;
		if(dst.x <= src.x && nowdir == Direction::Right) continue;

		const Point nowadj = src + Point::delta(nowdir);
		if(!nowadj.isIn(y, x, height, width)){
			continue;
		}

		const int dist = (nowadj - sel).l1norm();
		if(dist < minDist){
			dir = nowdir;
			adj = nowadj;
			minDist = dist;
		}
	}
	BOOST_ASSERT(adj.y >= 0);


	// 月蝕判定
	const bool eclipseY = (sel.y <= src.y && src.y < adj.y) || (adj.y < src.y && src.y <= sel.y);
	const bool eclipseX = (sel.x <= src.x && src.x < adj.x) || (adj.x < src.x && src.x <= sel.x);

	// 月蝕の位置で一直線に並んでいるケース
	// the special case
	if(sel.x == src.x && src.x == adj.x && eclipseY){
		sel.x == x ? board.moveRight() : board.moveLeft();
	}
	else if(sel.y == src.y && src.y == adj.y && eclipseX){
		sel.y == y ? board.moveDown() : board.moveUp();
	}

	// 月蝕の位置の方向から移動
	if(eclipseY){
		board.moveVertically(adj.y);
		board.moveHorizontally(adj.x);
	}
	else{
		board.moveHorizontally(adj.x);
		board.moveVertically(adj.y);
	}

	// move target
	for(;;){
		board.move(dir.opposite());
		if(board(dst) == pieceId){
			break;
		}

		switch(dir){
		case Direction::Up:
			if(board.selected.x == dst.x){
				dst.x == x ? board.moveRight() : board.moveLeft();
				board.moveUp();
				board.moveUp();
				dst.x == x ? board.moveLeft() : board.moveRight();
			}
			else if(board.selected.x < dst.x){
				board.moveRight();
				board.moveUp();
				dir = Direction::Right;
			}
			else{
				board.moveLeft();
				board.moveUp();
				dir = Direction::Left;
			}
			break;

		case Direction::Right:
			if(board.selected.y == dst.y){
				dst.y == y ? board.moveDown() : board.moveUp();
				board.moveRight();
				board.moveRight();
				dst.y == y ? board.moveUp() : board.moveDown();
			}
			else if(board.selected.y < dst.y){
				board.moveDown();
				board.moveRight();
				dir = Direction::Down;
			}
			else{
				board.moveUp();
				board.moveRight();
				dir = Direction::Up;
			}
			break;

		case Direction::Down:
			if(board.selected.x == dst.x){
				dst.x == x ? board.moveRight() : board.moveLeft();
				board.moveDown();
				board.moveDown();
				dst.x == x ? board.moveLeft() : board.moveRight();
			}
			else if(board.selected.x > dst.x){
				board.moveLeft();
				board.moveDown();
				dir = Direction::Left;
			}
			else{
				board.moveRight();
				board.moveDown();
				dir = Direction::Right;
			}
			break;

		case Direction::Left:
			if(board.selected.y == dst.y){
				dst.y == y ? board.moveDown() : board.moveUp();
				board.moveLeft();
				board.moveLeft();
				dst.y == y ? board.moveUp() : board.moveDown();
			}
			else if(board.selected.y < dst.y){
				board.moveDown();
				board.moveLeft();
				dir = Direction::Down;
			}
			else{
				board.moveUp();
				board.moveLeft();
				dir = Direction::Up;
			}
			break;
		}
	}
}

void StraightSolver::align2x2(AnswerBoard<Flexible>& board)
{
	BOOST_ASSERT(board.selected.isIn(2, 2));
	board.moveVertically(0);
	board.moveHorizontally(0);

	const uchar id = board.correctId(0, 1);
	if(board(1, 0) == id){
		board.moveDown();
		board.moveRight();
		board.moveUp();
		board.moveLeft();
	}
	else if(board(1, 1) == id){
		board.moveRight();
		board.moveDown();
		board.moveLeft();
		board.moveUp();
	}

	if(!board.isAligned(1, 0)){
		board.select(1, 0);
		board.moveRight();
	}
}

AnswerBoard<Flexible> StraightSolver::cropTo(
	const AnswerBoard<Flexible>& src, int y, int x, int height, int width, Direction topSide, bool yflip, bool xflip)
{
	Board<Flexible> dst(topSide.isUpOrDown() ? height : width, topSide.isUpOrDown() ? width : height);

	rep(i, height) rep(j, width){
		uchar id = src(i+y, j+x);
		id = Point(id).rotateFlip(topSide, yflip, xflip, height, width).toInt();
		dst(Point(i, j).rotateFlip(topSide, yflip, xflip, height, width)) = id;
	}

	AnswerBoard<Flexible> answerBoard(dst);
	answerBoard.selected = (src.selected - Point(y, x)).rotateFlip(topSide, yflip, xflip, height, width);

	return answerBoard;
}

void StraightSolver::restore(
	const AnswerBoard<Flexible>& src, AnswerBoard<Flexible>& dst, int y, int x, Direction topSide, bool yflip, bool xflip)
{
	const Direction preTopSide = topSide;
	topSide = topSide.isRightOrLeft() ? topSide.opposite() : topSide;

	rep(i, src.height()) rep(j, src.width()){
		uchar id = src(i, j);
		id = (Point(id).rotateFlip(topSide, yflip, xflip, src.height(), src.width()) + Point(y, x)).toInt();
		dst.at(Point(i, j).rotateFlip(topSide, yflip, xflip, src.height(), src.width()) + Point(y, x)) = id;
	}
	dst.selected = src.selected.rotateFlip(topSide, yflip, xflip, src.height(), src.width()) + Point(y, x);

	// answer の restore
	for(const Move& move : src.answer){
		if(move.isSelection){
			dst.answer.emplace_back(move.getSelected().rotateFlip(topSide, yflip, xflip, src.height(), src.width()) + Point(y, x));
		}
		else{
			Direction dir = move.getDirection();
			if(yflip && dir.isUpOrDown())    dir = dir.opposite();
			if(xflip && dir.isRightOrLeft()) dir = dir.opposite();
			dst.answer.emplace_back(dir + preTopSide);
		}
	}
}

void StraightSolver::solve()
{
	// initialize AnswerBoard
	AnswerBoard<Flexible> board(problem.board);
	board.select(board.find(board.correctId(0, 0)));

	// continue rearrangement until the remaining region is larger than 2x2
	int remH = board.height();
	int remW = board.width();

	while(remH > 2 || remW > 2){
		const bool isRow = remH >= remW;

		if(isRow){
			AnswerBoard<Flexible> tmp = cropTo(board, 0, 0, remH, remW, Direction::Up, false, false);
			alignRow(tmp);
			restore(tmp, board, 0, 0, Direction::Up, false, false);
			--remH;
		}
		else{
			AnswerBoard<Flexible> tmp = cropTo(board, 0, 0, remH, remW, Direction::Left, false, false);
			alignRow(tmp);
			restore(tmp, board, 0, 0, Direction::Left, false, false);
			--remW;
		}
	}

	align2x2(board);

	board.answer.optimize();
	onCreatedAnswer(board.answer);
	std::cout << "move_count = " << board.answer.size() << std::endl;
}

} // end of namespace slide
