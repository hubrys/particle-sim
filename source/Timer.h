#pragma once

#include <chrono>

class Timer
{
public:
	void reset();
	float lap();
	float total();

private:
	std::chrono::steady_clock::time_point _start;
	std::chrono::steady_clock::time_point _last;
};

inline void Timer::reset() 
{
	_start = std::chrono::steady_clock::now();
	_last = _start;
}

inline float Timer::lap()
{
	using namespace std::chrono;
	steady_clock::time_point now = steady_clock::now();
	steady_clock::duration duration = now - _last;
	_last = now;

	return float(duration.count()) * steady_clock::period::num / steady_clock::period::den;
}