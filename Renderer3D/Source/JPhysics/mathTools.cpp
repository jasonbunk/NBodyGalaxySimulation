/*
 * Author: Jason Bunk
 * Web page: http://sites.google.com/site/jasonbunk
 * Copyright (c) 2015 Jason Bunk
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "stdafx.h"
#include "mathTools.h"

//#include <random>
#include <math.h>
#include <float.h>
#include <stdlib.h> //rand


#define COMPARE_FLOATS_EPS(f1,f2,epsilon) (((f1-epsilon)<f2)&&((f1+epsilon)>f2))

const float FLT_EPSILON_NEW = (FLT_EPSILON * 2.0); //ehhh, not a big deal (it's complicated)
const double DBL_EPSILON_NEW = (DBL_EPSILON * 2.0);


namespace phys
{
	namespace mathTools
	{
		const JPHYS_FLOAT_UNIT E              = 2.71828182845904523536;
		const JPHYS_FLOAT_UNIT TWO_PI         = 6.28318530717958647693; //~18-21 decimal places (true "doubles" have ~15-16 decimal places)
		const JPHYS_FLOAT_UNIT NEGATIVE_TWO_PI=-6.28318530717958647693;
		const JPHYS_FLOAT_UNIT PI             = 3.14159265358979323846;
		const JPHYS_FLOAT_UNIT ONE_HALF_PI    = 1.57079632679489661923;
		const JPHYS_FLOAT_UNIT ONE_THIRD_PI   = 1.04719755119659774615;
		const JPHYS_FLOAT_UNIT ONE_FOURTH_PI  = 0.78539816339744830962;
		const JPHYS_FLOAT_UNIT ONE_SIXTH_PI   = 0.52359877559829887308;
		const JPHYS_FLOAT_UNIT ROOT_TWO       = 1.41421356237309504880;
		const JPHYS_FLOAT_UNIT RAD_TO_DEG     = 57.2957795130823208768;
		const JPHYS_FLOAT_UNIT DEG_TO_RAD     = 0.01745329251994329577;

		
		bool cmpFloatsWithRange(float f1, float f2, float allowed_range)
		{
			return COMPARE_FLOATS_EPS(f1, f2, allowed_range);
		}
		bool cmpFloats(float f1, float f2)
		{
			float eps = FLT_EPSILON*(f1+f2); //2x the average of them: allows 2x the inaccuracy of FLT_EPSILON
												//since FLT_EPSILON is for values around 1.0, this scales to larger or smaller floats

			return COMPARE_FLOATS_EPS(f1, f2, eps);
		}
		bool cmpFloatToZero(float f1)
		{
			return (fabs(f1) < FLT_EPSILON_NEW);
		}
		bool cmpFloatToZeroWithRange(float f1, float allowed_range)
		{
			return (fabs(f1) < allowed_range);
		}
		
		bool cmpDoublesWithRange(double f1, double f2, double allowed_range)
		{
			return COMPARE_FLOATS_EPS(f1, f2, allowed_range);
		}
		bool cmpDoubles(double f1, double f2)
		{
			double eps = DBL_EPSILON*(f1+f2); //2x the average of them: allows 2x the inaccuracy of DBL_EPSILON
												//since DBL_EPSILON is for values around 1.0, this scales to larger or smaller doubles

			return COMPARE_FLOATS_EPS(f1, f2, eps);
		}
		bool cmpDoubleToZero(double f1)
		{
			return (fabs(f1) < ((double)DBL_EPSILON_NEW));
		}
		bool cmpDoubleToZeroWithRange(double f1, double allowed_range)
		{
			return (fabs(f1) < allowed_range);
		}

		double RandomD(double min, double max)
		{
			if(min!=max)
				return (min + ((max-min)*(((double)rand())/((double)RAND_MAX))));
			return min;
		}
		float RandomF(float min, float max)
		{
			if(min!=max)
				return (min + ((max-min)*(((float)rand())/((float)RAND_MAX))));
			return min;
		}
		int RandomI(int min, int max)
		{
			return min + (rand() % (int)(max - min + 1));
		}
		
		JPHYS_FLOAT_UNIT differenceBetweenAnglesSigned(double x, double y)
		{
			return atan2(sin(x-y), cos(x-y));
		}

		double RoundDoubleToInteger(double num)
		{
			if ((num - floor(num)) < 0.5)
				return floor(num);
			else
				return ceil(num);
		}
		float RoundFloatToInteger(float num)
		{
			if ((num - floor(num)) < 0.5)
				return floor(num);
			else
				return ceil(num);
		}

		int RoundFloatToInt(float x)
		{
			return static_cast<int>((x>=0.0) ? floor(x+0.5) : ceil(x-0.5));
		}
		short RoundFloatToShort(float x)
		{
			return static_cast<short>((x>=0.0) ? floor(x+0.5) : ceil(x-0.5));
		}
		char RoundFloatToChar(float x)
		{
			return static_cast<char>((x>=0.0) ? floor(x+0.5) : ceil(x-0.5));
		}
		unsigned char RoundFloatToUnsignedChar(float x)
		{
			return static_cast<unsigned char>((x>=0.0) ? floor(x+0.5) : ceil(x-0.5));
		}

		
		int RoundDoubleToInt(double x)
		{
			return static_cast<int>((x>=0.0) ? floor(x+0.5) : ceil(x-0.5));
		}
		short RoundDoubleToShort(double x)
		{
			return static_cast<short>((x>=0.0) ? floor(x+0.5) : ceil(x-0.5));
		}
		char RoundDoubleToChar(double x)
		{
			return static_cast<char>((x>=0.0) ? floor(x+0.5) : ceil(x-0.5));
		}
		unsigned char RoundDoubleToUnsignedChar(double x)
		{
			return static_cast<unsigned char>((x>=0.0) ? floor(x+0.5) : ceil(x-0.5));
		}
	}
}
