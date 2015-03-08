/*
    original source from:
    nbody_kernel.cl
    Copyright 2009-2010 Brown Deer Technology, LLC.
    http://www.browndeertechnology.com/docs/BDT_OpenCL_Tutorial_NBody-rev3.html
   
   modified to use Verlet integration
   
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

#define NEWTONS_GRAVITY_CONSTANT 1.0f

__kernel void nbody_kern_func_main(
				const float dt1,		//arg 0
				const float epssqd,		//arg 1
				__global float4* pos_stepminus1,//arg 2
				__global float4* pos_curstep,	//arg 3
				__global float4* pos_nextstep,	//arg 4
				__local float4* pblock)		//arg 5
{
	const float4 dt = (float4)(dt1,dt1,dt1,0.0f);
	int gti = get_global_id(0); //index of the particle that this kernel will update (global id of this work item)
	int ti = get_local_id(0);   //local index, one of the work items for that particle
	int n = get_global_size(0); //num particles (total, global num work items)
	int nt = get_local_size(0); //work-group size (num work-items in a work-group) (nthreads)
	int nb = n/nt;              //calculate number of "blocks" (work-groups)
	
	float4 pminus1 = pos_stepminus1[gti];
	float4 p = pos_curstep[gti];
	float4 a = (float4)(0.0f,0.0f,0.0f,0.0f);
	
	for(int jb=0; jb < nb; jb++) {      /* Foreach block ... */
	   pblock[ti] = pos_curstep[jb*nt+ti];  /* Cache ONE particle position */
	   barrier(CLK_LOCAL_MEM_FENCE);    /* Wait for others in the work-group */
	   for(int j=0; j<nt; j++) {        /* For ALL cached particle positions... */
		float4 p2 = pblock[j];      /* Read a cached particle position */
		float4 d = p2 - p;
		float invr = rsqrt(d.x*d.x + d.y*d.y + d.z*d.z + epssqd);
		float f = NEWTONS_GRAVITY_CONSTANT*p2.w*invr*invr*invr;
		a += f*d;                   /* Accumulate acceleration */
	   }
	   barrier(CLK_LOCAL_MEM_FENCE); /* Wait for others in work-group */
	}
	p = (2.0f*p - pminus1 + dt*dt*a);
	
	pos_nextstep[gti] = p;
}

