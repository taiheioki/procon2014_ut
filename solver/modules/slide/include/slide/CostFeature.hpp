#ifndef SLIDE_COST_FEATURE_HPP_
#define SLIDE_COST_FEATURE_HPP_

#include "Problem.hpp"

namespace slide
{

class CostFeature
{
public:
	int selectionLimit;
	int cost;

	CostFeature() = default;
	CostFeature(int selectionLimit){
		init(selectionLimit);
	}

	void init(int selectionLimit, int cost = 0)
	{
		this->selectionLimit = selectionLimit;
		this->cost = cost;
	}

	void move()
	{
		cost += swappingCost;
	}

	void select()
	{
		cost += selectionCost;
		--selectionLimit;
	}

    /**************************************************************************
     * static variables and functions
     *************************************************************************/

	static int swappingCost;
	static int selectionCost;

	static void setCosts(int newSwappingCost, int newSelectionCost)
	{
		swappingCost = newSwappingCost;
		selectionCost = newSelectionCost;
	}
};

} // end of namespace slide

#endif
