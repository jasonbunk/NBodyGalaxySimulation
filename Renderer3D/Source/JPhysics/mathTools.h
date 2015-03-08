
#ifndef __JPHYSICS_MATH_CONSTANTS_H__
#define __JPHYSICS_MATH_CONSTANTS_H__

//#include "exports.h"
#define JPHYSICS_API
#define JPHYS_FLOAT_UNIT double
#include <math.h>
#include "Templates.h"


namespace phys
{

	namespace mathTools
	{
		extern JPHYSICS_API const JPHYS_FLOAT_UNIT E;
		extern JPHYSICS_API const JPHYS_FLOAT_UNIT TWO_PI;
		extern JPHYSICS_API const JPHYS_FLOAT_UNIT NEGATIVE_TWO_PI;
		extern JPHYSICS_API const JPHYS_FLOAT_UNIT PI;
		extern JPHYSICS_API const JPHYS_FLOAT_UNIT ONE_HALF_PI;
		extern JPHYSICS_API const JPHYS_FLOAT_UNIT ONE_THIRD_PI;
		extern JPHYSICS_API const JPHYS_FLOAT_UNIT ONE_FOURTH_PI;
		extern JPHYSICS_API const JPHYS_FLOAT_UNIT ONE_SIXTH_PI;
		extern JPHYSICS_API const JPHYS_FLOAT_UNIT ROOT_TWO;
		extern JPHYSICS_API const JPHYS_FLOAT_UNIT RAD_TO_DEG; //multiply radian-angles by this to turn them into degrees
		extern JPHYSICS_API const JPHYS_FLOAT_UNIT DEG_TO_RAD; //multiply degree-angles by this to turn them into radians

		
		//inclusive (includes min and max as possible values)
		JPHYSICS_API double	RandomD(double min, double max);
		JPHYSICS_API float	RandomF(float min, float max);
		JPHYSICS_API int	RandomI(int min, int max);
		
		JPHYSICS_API bool cmpFloats(float f1, float f2);
		JPHYSICS_API bool cmpDoubles(double f1, double f2);
		
		JPHYSICS_API bool cmpFloatsWithRange(float f1, float f2, float allowed_range);
		JPHYSICS_API bool cmpDoublesWithRange(double f1, double f2, double allowed_range);

		//returns true if it's zero (or extremely close to zero)
		JPHYSICS_API bool cmpFloatToZero(float f1);
		JPHYSICS_API bool cmpDoubleToZero(double f1);
		
		JPHYSICS_API INLINE bool cmpFloatToZeroWithRange(float f1, float allowed_range);
		JPHYSICS_API INLINE bool cmpDoubleToZeroWithRange(double f1, double allowed_range);
		
		JPHYSICS_API JPHYS_FLOAT_UNIT differenceBetweenAnglesSigned(double x_source, double y_subtracted);
		
		JPHYSICS_API double RoundDoubleToInteger(double num);
		JPHYSICS_API float RoundFloatToInteger(float num);

		JPHYSICS_API int RoundFloatToInt(float x);
		JPHYSICS_API short RoundFloatToShort(float x);
		JPHYSICS_API char RoundFloatToChar(float x);
		JPHYSICS_API unsigned char RoundFloatToUnsignedChar(float x);
		
		JPHYSICS_API int RoundDoubleToInt(double x);
		JPHYSICS_API short RoundDoubleToShort(double x);
		JPHYSICS_API char RoundDoubleToChar(double x);
		JPHYSICS_API unsigned char RoundDoubleToUnsignedChar(double x);
	}
}

#endif
