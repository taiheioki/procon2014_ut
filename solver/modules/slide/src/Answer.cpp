#include <algorithm>
#include <sstream>
#include <utility>

#include <boost/format.hpp>

#include "Answer.hpp"

namespace slide
{

void Answer::optimize()
{
	if(empty()){
		return;
	}

	Answer newans;
	newans.reserve(size());

	// 最初の選択操作
	const_iterator itr = begin();
	BOOST_ASSERT(itr->isSelection);
	newans.push_back(*itr);
	Point sel = itr->getSelected();
	++itr;

	int rot = 0;

	while(itr != end()){
		const Move pre = *newans.rbegin();

		if(itr->isSelection){
			if(sel == itr->getSelected()){
				// 同じ所を選択しようとした
				++itr;
				continue;
			}

			if(pre.isSelection){
				// 連続した選択操作を縮約
				newans.pop_back();
			}
			
			newans.push_back(*itr);
			sel = itr->getSelected();
			rot = 0;
		}
		else{
			if(pre.isSelection){
				newans.push_back(itr->getDirection());
			}
			else{
				const Direction dir = itr->getDirection() - pre.getDirection();

				// 反対向きに動くのを消す
				if(dir == Direction::Down){
					newans.pop_back();
					if(rot > 0) --rot;
					else if(rot < 0) ++rot;
				}
				else{
					newans.push_back(itr->getDirection());

					if(rot >= 0 && dir == Direction::Right){
						++rot;

						if(rot == 6){
							rep(i, 7) newans.pop_back();
							Direction newdir = itr->getDirection().ccw();
							rep(i, 5){
								newans.push_back(newdir);
								newdir = newdir.ccw();
							}
							rot = -4;
						}
					}
					else if(rot <= 0 && dir == Direction::Left){
						--rot;

						if(rot == -6){
							rep(i, 7) newans.pop_back();
							Direction newdir = itr->getDirection().cw();
							rep(i, 5){
								newans.push_back(newdir);
								newdir = newdir.cw();
							}
							rot = 4;
						}
					}
					else{
						if(dir == Direction::Up) rot = 0;
						else if(dir == Direction::Right) rot = 1;
						else rot = -1;
					}
				}
			}

			sel += Point::delta(itr->getDirection());
		}
		++itr;
	}

	// 選択操作しかしてない
	if(std::all_of(newans.begin(), newans.end(), [](const Move move){ return move.isSelection; })){
		clear();
		return;
	}

	*this = std::move(newans);
}

std::ostream& operator<<(std::ostream& out, const Answer& ans)
{
	const int selectionNum = std::count_if(ans.begin(), ans.end(), [](Move m){ return m.isSelection; });
	out << selectionNum << '\n';

	for(Answer::const_iterator itr = ans.begin(); itr != ans.end();){
		const Point p(itr->getSelected());
		out << boost::format("%02X\n") % int(Point(p.x, p.y).toInt()); // swapping x and y
		++itr;

		const int swappingNum = std::find_if(itr, ans.end(), [](const Move m){ return m.isSelection; }) - itr;
		out << swappingNum << '\n';

		rep(i, swappingNum){
			out << "URDL"[static_cast<uchar>(itr->getDirection())];
			++itr;
		}
		out << '\n';
	}

	return out;
}

} // end of namespace slide
