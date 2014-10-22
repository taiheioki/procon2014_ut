#ifndef UTIL_RANDOM_HPP_
#define UTIL_RANDOM_HPP_

#include <random>
#include <vector>

#include <boost/assert.hpp>

#include "SpinMutex.hpp"
#include "ThreadIndexManager.hpp"

namespace util
{

class Random
{
private:
	static std::vector<Random> random;
	static util::SpinMutex mutex;

public:
	Random() : engine(std::random_device{}()) {}
	std::default_random_engine engine;

	static void seed(unsigned int seed, int thread = util::ThreadIndexManager::getLocalId())
	{
		getEngine(thread).seed(seed);
	}
	
	static int nextInt(int min, int max, int thread = util::ThreadIndexManager::getLocalId())
	{
		std::uniform_int_distribution<> dist(min, max);
		return dist(getEngine(thread));
	}

	static unsigned long long nextULL(int thread = util::ThreadIndexManager::getLocalId())
	{
		std::uniform_int_distribution<unsigned long long> dist(
			std::numeric_limits<unsigned long long>::min(),
			std::numeric_limits<unsigned long long>::max()
		);
		return dist(getEngine(thread));
	}

	static float nextReal(int thread = util::ThreadIndexManager::getLocalId())
	{
		std::uniform_real_distribution<> dist;
		return dist(getEngine(thread));
	}

	static std::default_random_engine& getEngine(int thread = util::ThreadIndexManager::getLocalId())
	{
		if(random.size() <= thread){
            std::lock_guard<util::SpinMutex> lock(mutex);
			if(random.size() <= thread){
                random.resize(thread + 1);
            }
        }

		return random[thread].engine;
	}
};

} // end of namespace util

#endif
