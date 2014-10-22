#include <cstdlib>
#include <iostream>

#include "slide/Answer.hpp"


int main()
{
	std::cout << "連続した選択" << std::endl;
	slide::Answer ans;
	ans.emplace_back(slide::Point(0, 0));
	ans.emplace_back(slide::Point(1, 0));
	ans.emplace_back(slide::Point(0, 3));
	ans.emplace_back(slide::Direction::Down);
	ans.emplace_back(slide::Point(2, 0));
	ans.emplace_back(slide::Point(0, 8));
	ans.emplace_back(slide::Direction::Left);
	std::cout << "before : " << ans << std::endl;
	ans.optimize();
	std::cout << "after  : " << ans << std::endl;

	std::cout << "同じ場所の選択" << std::endl;
	ans.clear();
	ans.emplace_back(slide::Point(0, 0));
	ans.emplace_back(slide::Direction::Up);
	ans.emplace_back(slide::Direction::Right);
	ans.emplace_back(slide::Direction::Down);
	ans.emplace_back(slide::Direction::Left);
	ans.emplace_back(slide::Point(0, 0));
	ans.emplace_back(slide::Direction::Left);
	std::cout << "before : " << ans << std::endl;
	ans.optimize();
	std::cout << "after  : " << ans << std::endl;

	// 折り返しの削除
	std::cout << "折り返し" << std::endl;
	ans.clear();
	ans.emplace_back(slide::Point(0, 0));
	ans.emplace_back(slide::Direction::Up);
	ans.emplace_back(slide::Direction::Down);
	ans.emplace_back(slide::Direction::Right);
	ans.emplace_back(slide::Direction::Right);
	ans.emplace_back(slide::Direction::Left);
	ans.emplace_back(slide::Direction::Up);
	ans.emplace_back(slide::Direction::Down);
	ans.emplace_back(slide::Direction::Left);
	ans.emplace_back(slide::Direction::Right);
	std::cout << "before : " << ans << std::endl;
	ans.optimize();
	std::cout << "after  : " << ans << std::endl;

	rep(i, 20){
		std::cout << i << "回転" << std::endl;

		ans.clear();
		ans.emplace_back(slide::Point(0, 0));

		slide::Direction dir = slide::Direction::Up;
		rep(j, i){
			ans.emplace_back(dir);
			dir = dir.cw();
		}

		std::cout << "before : " << ans << std::endl;
		ans.optimize();
		std::cout << "after  : " << ans << std::endl;
	}

	std::cout << "回転と折り返し" << std::endl;
	ans.clear();
	ans.emplace_back(slide::Point(0, 0));
	rep(_, 2){
		ans.emplace_back(slide::Direction::Up);
		ans.emplace_back(slide::Direction::Up);
		ans.emplace_back(slide::Direction::Down);
		ans.emplace_back(slide::Direction::Right);
		ans.emplace_back(slide::Direction::Right);
		ans.emplace_back(slide::Direction::Left);
		ans.emplace_back(slide::Direction::Down);
		ans.emplace_back(slide::Direction::Down);
		ans.emplace_back(slide::Direction::Up);
		ans.emplace_back(slide::Direction::Left);
	}
	std::cout << "before : " << ans << std::endl;
	ans.optimize();
	std::cout << "after  : " << ans << std::endl;

	std::cout << "回転と折り返し2" << std::endl;
	ans.clear();
	ans.emplace_back(slide::Point(0, 0));
	ans.emplace_back(slide::Direction::Down);
	rep(_, 3){
		ans.emplace_back(slide::Direction::Up);
		ans.emplace_back(slide::Direction::Right);
		ans.emplace_back(slide::Direction::Down);
		ans.emplace_back(slide::Direction::Left);
	}
	ans.emplace_back(slide::Direction::Up);
	std::cout << "before : " << ans << std::endl;
	ans.optimize();
	std::cout << "after  : " << ans << std::endl;

	std::cout << "ラスボス" << std::endl;
	ans.clear();
	ans.emplace_back(slide::Point(8, 8));
	ans.emplace_back(slide::Direction::Left);
	ans.emplace_back(slide::Direction::Left);
	ans.emplace_back(slide::Direction::Left);
	ans.emplace_back(slide::Direction::Left);
	ans.emplace_back(slide::Direction::Up);
	ans.emplace_back(slide::Direction::Right);
	ans.emplace_back(slide::Direction::Left);
	ans.emplace_back(slide::Direction::Down);
	ans.emplace_back(slide::Direction::Right);
	ans.emplace_back(slide::Direction::Up);
	ans.emplace_back(slide::Direction::Right);
	ans.emplace_back(slide::Direction::Down);
	ans.emplace_back(slide::Direction::Left);
	ans.emplace_back(slide::Direction::Left);
	ans.emplace_back(slide::Direction::Up);
	std::cout << "before : " << ans << std::endl;
	ans.optimize();
	std::cout << "after  : " << ans << std::endl;

	return EXIT_SUCCESS;
}
