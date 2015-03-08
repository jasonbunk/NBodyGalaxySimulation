#pragma once

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.


// platform detection

#define PLATFORM_WINDOWS  1
#define PLATFORM_MAC      2
#define PLATFORM_LINUX     3
#define PLATFORM_UNSUPPORTED 4

#if defined(_WIN32)
#define THISPLATFORM PLATFORM_WINDOWS
#pragma message("This is now compiling for Windows platform!")
#elif defined(__APPLE__)
#define THISPLATFORM PLATFORM_MAC
#pragma message("This is now compiling for Apple Mac platform!")
#elif defined(__linux__)
#define THISPLATFORM PLATFORM_LINUX
#pragma message("This is now compiling for Linux platform!")
#else
#define THISPLATFORM PLATFORM_UNSUPPORTED
#pragma message("This is now compiling for some unknown, unsupported platform!")
#endif



#if THISPLATFORM == PLATFORM_LINUX
#define INLINE
#define __forceinline
#else
#define INLINE
#endif


#ifndef __stricmp

#if THISPLATFORM == PLATFORM_WINDOWS
#pragma message("strings building for Windows")
#define __stricmp(x,y) _stricmp(x,y)
#else
#pragma message("strings building for Linux")
#include <strings.h>
#define __stricmp(x,y) strcasecmp(x,y)
#endif

#endif
