#ifndef ___TRY_INCLUDE_GLEW_H_______
#define ___TRY_INCLUDE_GLEW_H_______


#if THISPLATFORM == PLATFORM_WINDOWS
#define GLEW_STATIC
#include <GL/glew.h>
#else
#define GLEW_STATIC
#include <GL/glew.h>
//#include <glew-1.9.0/include/GL/glew.h>
#endif


#endif
