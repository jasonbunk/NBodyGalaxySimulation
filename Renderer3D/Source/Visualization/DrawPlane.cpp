/*
 * Draw a grid using OpenGL.
 *
 * Author: Jason Bunk
 * Web page: http://sites.google.com/site/jasonbunk
 * License: Apache License Version 2.0, January 2004
 * Copyright (c) 2015 Jason Bunk
 */
#include "stdafx.h"
#include "DrawPlane.h"

namespace draw_planes
{


void Draw(plane whichplane, int numlines, float extent, phys::vec3 origin, double opacity, double lightness)
{
	Draw(whichplane, numlines, static_cast<double>(extent), origin, opacity, lightness);
}

void Draw(plane whichplane, int numlines, double extent, phys::vec3 origin, double opacity, double lightness)
{
	glBegin(GL_LINES);
	unsigned char lightness_value = static_cast<unsigned char>(255.0 * lightness);
	unsigned char opacity_value = static_cast<unsigned char>(255.0 * opacity);
	glColor4ub(lightness_value, lightness_value, lightness_value, opacity_value);

	if(numlines > 1)
	{
		int aa = 0;
		double writePos = -extent;
		double WI = (extent * 2.0) / static_cast<double>((numlines - 1));

		switch(whichplane)
		{
		case draw_planes::xy:
			for(; aa < numlines; aa++)
			{
				glVertex3d(origin.x-extent, origin.y+writePos, origin.z);
				glVertex3d(origin.x+extent, origin.y+writePos, origin.z);

				glVertex3d(origin.x+writePos, origin.y-extent, origin.z);
				glVertex3d(origin.x+writePos, origin.y+extent, origin.z);

				writePos += WI;
			}
			break;
		case draw_planes::xz:
			for(; aa < numlines; aa++)
			{
				glVertex3d(origin.x-extent, origin.y, origin.z+writePos);
				glVertex3d(origin.x+extent, origin.y, origin.z+writePos);

				glVertex3d(origin.x+writePos, origin.y, origin.z-extent);
				glVertex3d(origin.x+writePos, origin.y, origin.z+extent);

				writePos += WI;
			}
			break;
		case draw_planes::yz:
			for(; aa < numlines; aa++)
			{
				glVertex3d(origin.x, origin.y-extent, origin.z+writePos);
				glVertex3d(origin.x, origin.y+extent, origin.z+writePos);

				glVertex3d(origin.x, origin.y+writePos, origin.z-extent);
				glVertex3d(origin.x, origin.y+writePos, origin.z+extent);

				writePos += WI;
			}
			break;
		}
	}
	else
	{
		switch(whichplane)
		{
		case draw_planes::xy:
			glVertex3d(origin.x-extent,	origin.y,			origin.z);
			glVertex3d(origin.x+extent,	origin.y,			origin.z);
			glVertex3d(origin.x,		origin.y-extent,	origin.z);
			glVertex3d(origin.x,		origin.y+extent,	origin.z);
			break;
		case draw_planes::xz:
			glVertex3d(origin.x,		origin.y,			origin.z-extent);
			glVertex3d(origin.x,		origin.y,			origin.z+extent);
			glVertex3d(origin.x-extent,	origin.y,			origin.z);
			glVertex3d(origin.x+extent,	origin.y,			origin.z);
			break;
		case draw_planes::yz:
			glVertex3d(origin.x,		origin.y-extent,	origin.z);
			glVertex3d(origin.x,		origin.y+extent,	origin.z);
			glVertex3d(origin.x,		origin.y,			origin.z-extent);
			glVertex3d(origin.x,		origin.y,			origin.z+extent);
			break;
		}
	}


	glEnd();
}


}
