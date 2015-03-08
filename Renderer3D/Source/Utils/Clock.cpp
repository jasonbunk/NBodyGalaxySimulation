#include "Clock.h"

double myclock::fixed_time_step___if_less_than_zero_all_clocks_are_realtime = -1.0;


double myclock::restart()
{
	trueClock.restart();
}

double myclock::getTimeSinceLastMeasurement()
{
	if(fixed_time_step___if_less_than_zero_all_clocks_are_realtime <= 0.0) {
		return (double)(trueClock.restart().asSeconds());
	} else {
		return fixed_time_step___if_less_than_zero_all_clocks_are_realtime;
	}
}
