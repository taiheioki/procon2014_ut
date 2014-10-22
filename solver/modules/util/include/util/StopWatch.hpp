#ifndef UTIL_STOP_WATCH_HPP_
#define UTIL_STOP_WATCH_HPP_

#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <ratio>
#include <stack>
#include <string>
#include <tuple>
#include <vector>

#include <boost/assert.hpp>
#include <boost/format.hpp>

#include "util/dense_hash_map.hpp"
#include "util/hash.hpp"

namespace util
{

#ifdef ENABLE_TIMER

class StopWatch
{
    /**************************************************************************
     * Type definitions and constants
     *************************************************************************/

public:
	using Duration = std::chrono::microseconds;

private:
	using TimePoint = std::chrono::high_resolution_clock::time_point;
	using PeriodRatio = std::ratio_divide<typename Duration::period, std::chrono::milliseconds::period>;

	static constexpr double TIME_RATIO = double(PeriodRatio::num) / PeriodRatio::den;


    /**************************************************************************
     * For non-static use
     *************************************************************************/

private:
	TimePoint before;
	Duration saved;
	bool isRunning;
	
public:
	StopWatch() : saved(0), isRunning(false) {}

	void start()
	{
		if(!isRunning){
			before = std::chrono::high_resolution_clock::now();
			isRunning = true;
		}
	}

	void stop()
	{
		if(isRunning){
			const TimePoint now = std::chrono::high_resolution_clock::now();
			saved += std::chrono::duration_cast<Duration>(now - before);
			isRunning = false;
		}
	}

	void reset()
	{
		saved = Duration(0);
		isRunning = false;
	}

	Duration elapsed() const
	{
		return isRunning ? saved + std::chrono::duration_cast<Duration>(std::chrono::high_resolution_clock::now() - before) : saved;
	}

	double elapsed_ms() const
	{
		return elapsed().count() * TIME_RATIO;
	}
	
	friend std::ostream& operator<<(std::ostream& out, const StopWatch& sw)
	{
		return out << sw.elapsed_ms() << "ms";
	}


    /**************************************************************************
     * For static use
     *************************************************************************/

private:
	using tag_type = std::pair<std::string, int>;
	using hash_map = dense_hash_map<tag_type, std::size_t, hash<tag_type>>;

	static hash_map orders;
	static std::vector<std::tuple<StopWatch, int>> table;
	static std::stack<int> trace;

	static int get_parent()
	{
		return trace.empty() ? -1 : trace.top();
	}

	static std::size_t get_order(const std::string& tag)
	{
		const hash_map::const_iterator itr = orders.find({tag, get_parent()});
		BOOST_ASSERT(itr != orders.end());
		return itr->second;
	}

	static std::size_t get_or_create_order(const std::string& tag)
	{
		const int parent = get_parent();
		hash_map::iterator itr = orders.find({tag, parent});
		if(itr == orders.end()){
			itr = orders.insert({{tag, parent}, table.size()}).first;
			table.emplace_back(StopWatch(), trace.size());
		}
		return itr->second;
	}

	static StopWatch& getStopWatch(const std::string& tag)
	{
		return std::get<0>(table[get_order(tag)]);
	}

public:
	static void start(const std::string& tag)
	{
		const int id = get_or_create_order(tag);
		std::get<0>(table[id]).start();
		trace.push(id);
	}

	static void stop(const std::string& tag)
	{
		trace.pop();
		std::get<0>(table[get_order(tag)]).stop();
	}

	static void stop_last()
	{
		BOOST_ASSERT(!trace.empty());
		std::get<0>(table[trace.top()]).stop();
		trace.pop();
	}

	static void reset(const std::string& tag)
	{
		getStopWatch(tag).reset();
	}

	static Duration elapsed(const std::string& tag)
	{
		return getStopWatch(tag).elapsed();
	}

	static double elapsed_ms(const std::string& tag)
	{
		return getStopWatch(tag).elapsed_ms();
	}

	static void clear()
	{
		orders.clear();
		table.clear();
	}

	static void show(std::ostream& out = std::cerr)
	{
		using list_row       = std::array<std::string, 2>;
		std::vector<list_row> listing(orders.size());

		// compose strings
		for(hash_map::const_iterator itr = orders.begin(); itr != orders.end(); ++itr){
			const StopWatch& watch = std::get<0>(table[itr->second]);
			const int        lv    = std::get<1>(table[itr->second]);
			listing[itr->second][0] = (lv ? std::string(lv, '-') + ' ' : "") + itr->first.first;
			listing[itr->second][1] = (boost::format("%.3fms") % watch.elapsed_ms()).str();
		}

		// measure length
		std::array<int, 2> max_length = {{0, 0}};
		for(const list_row& row : listing){
			for(int i=0; i<2; ++i){
				max_length[i] = std::max<int>(max_length[i], row[i].size());
			}
		}

		// output to stream
		for(const list_row& row : listing){
			out << row[0] << std::string(max_length[0] - row[0].size(), ' ');
			out << ' ';
			out << std::string(max_length[1] - row[1].size(), ' ') << row[1];
			out << '\n';
		}
	}
};

#else

class StopWatch
{
public:
	typedef std::chrono::microseconds Duration;
	StopWatch() {}
	StopWatch(const char*) {}
	Duration elapsed() const { return Duration(); }
	friend std::ostream& operator<<(std::ostream& out, const StopWatch& sw) { return out; }
};

#endif

} // end of namespace util

#endif
