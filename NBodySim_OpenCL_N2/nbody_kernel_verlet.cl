//when loading this kernel, define stuff at the front of this text
// MYDEF_USE_DOUBLE_PRECISION
// MYDEF_NUM_PARTICLES_PER_THREAD

//#define _____SetupNxMtypeNumericArray(pretypegiven, sizemgiven) \
//	pretypegiven ## x ## sizemgiven
//#define CreateNxMtypeNumericArray(pretypegiven, sizemgiven) \
//	_____SetupNxMtypeNumericArray(pretypegiven, sizemgiven)

/*
   nbody_kernel_verlet.cl
   An O(N^2) nbody simulator using OpenCL for portable GPU computing, using Verlet integration.
   
   Copyright 2015 Jason Bunk
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
       http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#ifdef MYDEF_USE_DOUBLE_PRECISION
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#define myflt double
#define myflt4 double4
#else
#define myflt float
#define myflt4 float4
#endif

#define PARTSPERTHREAD MYDEF_NUM_PARTICLES_PER_THREAD


__kernel void nbody_kern_func_main(
				const myflt dt1,		//arg 0
				const myflt epssqd,		//arg 1
				const myflt NewtonsGravConstG,	//arg 2
				const int npadding,		//arg 3
				__global myflt4* pos_stepminus1,//arg 4
				__global myflt4* pos_curstep,	//arg 5
				__global myflt4* pos_nextstep,	//arg 6
				__local myflt4* pcache)		//arg 7
{
	const myflt4 dt = (myflt4)(dt1,dt1,dt1,0.0f);
	int gwiidx = get_global_id(0); //global id of this work item
	int lwgidx = get_local_id(0); //local index of the work-item within its work group
	int nthreads = get_global_size(0); //total, global number of work items
	const int THREADSPERBLOCK = get_local_size(0); //work-group size (num work-items in a work-group) (nthreads)
	int nblocks = nthreads/THREADSPERBLOCK; //calculate number of "blocks" (work-groups)
	int nparts = (PARTSPERTHREAD * nthreads);
	const int partsperblock = (THREADSPERBLOCK * PARTSPERTHREAD);
	
	int nrealparts = nparts-npadding;
	
	int ii,jb,kk, cacheoffs, myglobalrowidx, partsbeforethisblock;
	int partsbeforemeglobally = (gwiidx*PARTSPERTHREAD);
	
	myflt4 mypos[PARTSPERTHREAD] = {};
	myflt4 accel[PARTSPERTHREAD] = {};
	
	//initialize my cached positions by pulling from global device memory; if OpenCL had memcpy it would be used here
	for(ii=0; ii<PARTSPERTHREAD; ii++) {
		mypos[ii] = pos_curstep[partsbeforemeglobally+ii];
	}
	
	//start looping through all other positions, block-by-block, moving rightward across columns in the matrix-style diagram
	for(jb=0; jb<nblocks; jb++) {
		partsbeforethisblock = (jb*partsperblock);
		
		//thread simultaneously cache bits from global memory; if OpenCL had memcpy it would be used here
		for(ii=0; ii<PARTSPERTHREAD; ii++) {
			cacheoffs = lwgidx*PARTSPERTHREAD + ii;
			pcache[cacheoffs] = pos_curstep[partsbeforethisblock + cacheoffs];
		}
		barrier(CLK_LOCAL_MEM_FENCE); //wait for other threads in block
		
		for(ii=0; ii<PARTSPERTHREAD; ii++) { //for indices this thread manages...
			myglobalrowidx = partsbeforemeglobally+ii;
			if(myglobalrowidx < nrealparts) { //if this row isn't a padding particle
				for(kk=0; kk<partsperblock; kk++) { //loop over all other columns in this cache
					if((partsbeforethisblock+kk)<nrealparts && (partsbeforethisblock+kk)!=myglobalrowidx) { //if the column isn't padding, nor the as this row
						myflt4 p2 = pcache[kk];
						myflt4 d = p2 - mypos[ii];
						myflt invr = rsqrt(d.x*d.x + d.y*d.y + d.z*d.z + epssqd);
						myflt f = NewtonsGravConstG*p2.w*invr*invr*invr;
						accel[ii] += f*d;
					}
				}
			}
		}
		barrier(CLK_LOCAL_MEM_FENCE);
	}
	
	//todo: condense this without the "if" statement since we dont care where the ghost particles go
	//calculate next position using Verlet, and write it to global device memory
	for(ii=0; ii<PARTSPERTHREAD; ii++) {
		myglobalrowidx = partsbeforemeglobally+ii;
		mypos[ii] = (2.0f*mypos[ii] - pos_stepminus1[myglobalrowidx] + dt*dt*accel[ii]);
		if(myglobalrowidx < nrealparts) {
			pos_nextstep[myglobalrowidx] = mypos[ii];
		}
	}
}

