#ifndef CG_CLOCK_H_
#define CG_CLOCK_H_

#include <chrono>

namespace cg
{
class Clock
{
	decltype(std::chrono::steady_clock::now()) start;
	decltype(std::chrono::steady_clock::now()) lastStop;
	decltype(lastStop - start) lapTime;

public:
	Clock() : start(std::chrono::steady_clock::now()), lastStop(start), lapTime(0) {}

	void lap()
	{
		auto now = std::chrono::steady_clock::now();
		lapTime = now - lastStop;
		lastStop = now;
	}

	void reset()
	{
		start = std::chrono::steady_clock::now();
		lastStop = start;
		lapTime = lastStop - start;
	}

	template<typename TimeT = std::chrono::milliseconds>
	TimeT duration()
	{
		return std::chrono::duration_cast<TimeT>(std::chrono::steady_clock::now() - start);
	}

	template<typename TimeT = std::chrono::milliseconds>
	TimeT elapsed()
	{
		return std::chrono::duration_cast<TimeT>(lapTime);
	}

	double durationSecond()
	{
		return double(duration<std::chrono::nanoseconds>().count() * 1e-9);
	}

	double elapsedSecond()
	{
		return double(elapsed<std::chrono::nanoseconds>().count() * 1e-9);
	}
};
} /* namespace cg */

#endif /* CG_CLOCK_H_ */
