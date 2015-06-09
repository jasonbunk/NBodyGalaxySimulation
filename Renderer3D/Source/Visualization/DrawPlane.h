#ifndef __DRAW_PLANE_H___
#define __DRAW_PLANE_H___

#include "TryIncludeJPhysics.h"

namespace draw_planes
{
	enum plane
	{
		xy,
		xz,
		yz
	};
	
	void Draw(plane whichplane, int numlines, float extent_radius, phys::vec3 origin, double opacity, double lightness);
	void Draw(plane whichplane, int numlines, double extent_radius, phys::vec3 origin, double opacity, double lightness);
}


#endif
