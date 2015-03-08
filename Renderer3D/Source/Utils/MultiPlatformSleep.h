#ifndef ___MULTI_PLATFORM_SLEEP_H__
#define ___MULTI_PLATFORM_SLEEP_H__

#ifdef _WIN32
#include "Windows.h"
#include "WinBase.h"
#else
#include <chrono>
#include <thread>
#endif

void MultiPlatformSleep(int milliseconds);

#endif
