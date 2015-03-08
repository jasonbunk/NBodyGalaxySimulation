/* boxmuller.c           Implements the Polar form of the Box-Muller
                         Transformation

                      (c) Copyright 1994, Everett F. Carter Jr.
                          Permission is granted by the author to use
			  this software for any application provided this
			  copyright notice is preserved.
*/
#include "random_normal_dist_using_c.h"
#include <math.h>
#include <assert.h>
#include <iostream>

//==================================================
#if 0
#include <vector>
static std::vector<double> deviations;
#endif
//==================================================

// normal random variate generator
// mean m, standard deviation s

static int use_last = 0;
static double y2 = 0.0;

double normal_dist_box_muller(RNG* givenRNG, double m, double s, double clamped_num_stddevs /*= -1.0*/)
{
    assert(givenRNG != nullptr);
	double x1, x2, w, y1;
	double retval;

	if(use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do {
			x1 = (2.0*givenRNG->rand_double(0.0,1.0)) - 1.0;
			x2 = (2.0*givenRNG->rand_double(0.0,1.0)) - 1.0;
			w = x1*x1 + x2*x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}
	
	retval = (y1 * s);
	
	
#if 0
	deviations.push_back(retval);
	double sumsofar = 0.0;
	for(int ii=0; ii<deviations.size(); ii++) {
		sumsofar += fabs(pow(deviations[ii], 2.0));
	}
	sumsofar = sqrt(sumsofar/static_cast<double>(deviations.size()));
	std::cout<<"stddev calculated: "<<sumsofar<<std::endl;
#endif

	if(clamped_num_stddevs > 0.0) {
		if(fabs(retval/s) > clamped_num_stddevs) {
			//std::cout<<"clamped_num_stddevs == "<<clamped_num_stddevs<<",  retval == "<<retval<<",  s == "<<s<<std::endl;
			retval = (s*clamped_num_stddevs*retval/fabs(retval));
			//std::cout<<"\t\tresult == "<<retval<<std::endl;
		}
	}
	
	return m + retval;
}



