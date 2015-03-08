#ifndef ____UTILS_CLOCK_H________
#define ____UTILS_CLOCK_H________

#include <SFML/System/Clock.hpp>

class myclock
{
protected:
	sf::Clock trueClock;
public:
	static double fixed_time_step___if_less_than_zero_all_clocks_are_realtime;
	
	double restart();
	double getTimeSinceLastMeasurement();
};

#endif
