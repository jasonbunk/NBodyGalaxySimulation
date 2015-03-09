
#ifndef __JPHYSICS__TIMER____H______
#define __JPHYSICS__TIMER____H______

//#include "targetver.h"
#include "Utils/targetplatform.h"

/// Create a timer, which will immediately begin counting
/// up from 0.0 seconds.
/// You can call reset() to make it start over.

#if THISPLATFORM == PLATFORM_WINDOWS

//todo: since (supposedly) not all CPU's implement QueryPerformanceCounter,
//have an alternate GetTickCount() instead

#define NOMINMAX
#include <windows.h>

class TIMER
{
  public:
    TIMER()  { reset(); }

    /// reset() makes the timer start over counting from 0.0 seconds.
    void reset()
    {
      unsigned __int64 pf;
      QueryPerformanceFrequency( (LARGE_INTEGER *)&pf );
      freq_ = 1.0 / (double)pf;
      QueryPerformanceCounter( (LARGE_INTEGER *)&baseTime_ );
    }

    /// seconds() returns the number of seconds (to very high resolution)
    /// elapsed since the timer was last created or reset().
    double seconds()
    {
      unsigned __int64 val;
      QueryPerformanceCounter( (LARGE_INTEGER *)&val );
      return (val - baseTime_) * freq_;
    }

  private:
    double freq_;
    unsigned __int64 baseTime_;
};

#elseif THISPLATFORM == PLATFORM_LINUX

#include <time.h>

#ifndef syslog
#include <iostream>
#endif

class TIMER
{
public:
	TIMER() : my_timespec_current(NULL) {reset();}

	/// reset() makes the timer start over counting from 0.0 seconds.
	void reset()
	{
		if(my_timespec_current == NULL)
		{
			my_timespec_current = new timespec();
			my_timespec_current->tv_sec = 0;
			my_timespec_current->tv_nsec = 0;
			last_time_saved_ns = 0;
			ticktime_ns_to_s = 0.0;
		}
		if(clock_getres(CLOCK_MONOTONIC_RAW, my_timespec_current) == 0)
		{
			ticktime_ns_to_s = static_cast<double>(my_timespec_current->tv_nsec) * 0.000000001;

			clock_gettime(CLOCK_MONOTONIC_RAW, my_timespec_current);
			last_time_saved_ns = my_timespec_current->tv_nsec;
		}
		else
#ifdef syslog
			syslog.Print("\nwarning: TIMER couldn't reset()");
#else
            std::cout << "\nwarning: TIMER couldn't reset()";
#endif
	}

	/// seconds() returns the number of seconds (to very high resolution)
	/// elapsed since the timer was last created or reset().
	double seconds()
	{
		if(clock_gettime(CLOCK_MONOTONIC_RAW, my_timespec_current) == 0)
		{
			double retval = (static_cast<double>(my_timespec_current->tv_nsec - last_time_saved_ns) * ticktime_ns_to_s);
			if(retval < 0.0)
				return (retval + 1.0);

			return retval;
		}
		else
#ifdef syslog
			syslog.Print("\nwarning: TIMER couldn't get time");
#else
            std::cout << "\nwarning: TIMER couldn't get time";
#endif

		return 987654321.0;
	}

private:
	timespec* my_timespec_current;
	double ticktime_ns_to_s;
	long int last_time_saved_ns;
};

#else

#include <SFML/System/Clock.hpp>

class TIMER
{
public:
	sf::Clock myClock;

	/// reset() makes the timer start over counting from 0.0 seconds.
	void reset() {
	    myClock.restart();
	}

	/// seconds() returns the number of seconds (to very high resolution)
	/// elapsed since the timer was last created or reset().
	double seconds() {
	    return (double)myClock.getElapsedTime().asSeconds();
	}
};

#endif



#endif
