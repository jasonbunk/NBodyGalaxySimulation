#ifndef _RANDOM_DEVICE_NORMAL_DIST_USING_C_H____
#define _RANDOM_DEVICE_NORMAL_DIST_USING_C_H____

#include "SharedUtils_RNG.h"

double normal_dist_box_muller(RNG* givenRNG, double m, double s, double clamped_num_stddevs = 5.0);

#endif
