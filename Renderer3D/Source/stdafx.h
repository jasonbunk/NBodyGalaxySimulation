// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "Utils/targetplatform.h"

#if THISPLATFORM == PLATFORM_WINDOWS
#define NOMINMAX
#define _WINSOCKAPI_    // stops windows.h including winsock.h
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <SDKDDKVer.h>
#endif


#include "TryIncludeJPhysics.h"


#if THISPLATFORM == PLATFORM_WINDOWS
#include <Windows.h>
#include <tchar.h>
#else
#define TRUE 1
#define FALSE 0
#endif
#include <stdio.h>
#include <iostream>
#include <assert.h>


#if THISPLATFORM == PLATFORM_LINUX
#ifndef _stricmp
#include <strings.h>
#define _stricmp(x,y) strcasecmp(x,y)
#endif
#endif

#ifndef sprintf____s
#define sprintf____s sprintf
#endif






#include <limits>
// requires #include <limits>
template<typename T>
inline bool isINF(T value)
{
return std::numeric_limits<T>::has_infinity &&
value == std::numeric_limits<T>::infinity();
}

template<typename T>
inline bool isNaN(T value)
{
return ((value != value) || isINF(value));
}




#include "GameSystemBase.h"

extern GameSystemBase gGameSystem; //instantiated in "GameSystemBase.cpp"



// reference additional headers your program requires here
// frequently needed files, infrequently updated files


extern bool SystemIsPaused; //defined in main.cpp


void ExitSimulation(); //defined in main.cpp
void FakeResizeWindow(int w, int h, double zoom_factor); //defined in main.cpp




