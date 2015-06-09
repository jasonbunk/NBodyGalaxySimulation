/*
   nbodycpumpi.cpp
   Hybrid MPI/OpenMP gravitational O(N^2) nbody integrator using Verlet integration
   
   Copyright 2015 Jason Bunk; code available at https://github.com/jasonbunk/NBodyGalaxySimulation
   
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <vector>
using std::cout; using std::endl;

#include "mpi.h"
static int mpi_numCacheGroups; //command-line argument
static int mpi_myCacheNumber;
static int mpi_myIndexWithinCache;
static int mpi_numRanksWithinACache;
static int mpi_nextRank; //next in the ring, usually +1 except the last group for which is 0
static int mpi_prevRank; //previous in the ring
static int mpi_myrank; //global index, from which cache-relative indices are calculated
static int mpi_size; //number of global MPI processes
#define MPI_MESSAGE_TAG_POSITIONS 1
static int GlobalNumParticles; //in the universe; given by each initial conditions file's header
static int NumCachedParticles; //cached, usually shared by a few procs
static int NumLocalParticles; //for which this MPI proc updates positions


#ifdef USE_OMP
#include <omp.h>
#endif

#define DBLBYTES 8
#define FLTBYTES 4

static double NEWTONS_GRAVITY_CONSTANT = 1.0;


//=====================================================================================================
// Miscellaneous

int RoundDoubleToInt(double x) {
	return static_cast<int>((x>=0.0) ? floor(x+0.5) : ceil(x-0.5));
}
template <class T>
inline std::string to_istring(const T& t)
{
    std::stringstream ss;
    ss << static_cast<int>(t);
    return ss.str();
}
void MPIExit(int code) {
  MPI_Finalize();
  exit(code);
}

int AdvanceRankByCacheAmount(int mpirank) {
  int returnme = mpirank + mpi_numRanksWithinACache;
  while(returnme >= mpi_size) {
    returnme -= mpi_size;
  }
  return returnme;
}
int ReduceRankByCacheAmount(int mpirank) {
    int returnme = mpirank - mpi_numRanksWithinACache;
    while(returnme < 0) {
      returnme += mpi_size;
    }
    return returnme;
}
int AdvanceRankWithinCache(int mpirank) {
  int returnme = mpirank + 1;
  if(returnme % mpi_numRanksWithinACache == 0) {
    returnme -= mpi_numRanksWithinACache;
  }
  return returnme;
}
int ReduceRankWithinCache(int mpirank) {
  int returnme = mpirank - 1;
  if(returnme % mpi_numRanksWithinACache == (mpi_numRanksWithinACache-1)) {
    returnme += mpi_numRanksWithinACache;
  }
  return returnme;
}

class point {
public:
	double* ptr;
	
	point() : ptr(NULL) {}
	point(double* PTR) : ptr(PTR) {}
	
	double length() const {
		return sqrt(ptr[0]*ptr[0] + ptr[1]*ptr[1] + ptr[2]*ptr[2]);
	}
	double dotwith(const point& other) {
		return ptr[0]*other.ptr[0] + ptr[1]*other.ptr[1] + ptr[2]*other.ptr[2];
	}
	double & x() {return ptr[0];}
	double & y() {return ptr[1];}
	double & z() {return ptr[2];}
	double & at(int idx) {return ptr[idx];}
	void zero() {ptr[0]=0.0; ptr[1]=0.0; ptr[2]=0.0;}
  
	point& operator+=(const point& rhs) {
		ptr[0] += rhs.ptr[0];  ptr[1] += rhs.ptr[1];  ptr[2] += rhs.ptr[2];
		return *this;
	}
	friend point operator+(point lhs, const point& rhs) {
		return lhs += rhs;
	}
	point& operator-=(const point& rhs) {
		ptr[0] -= rhs.ptr[0];  ptr[1] -= rhs.ptr[1];  ptr[2] -= rhs.ptr[2];
		return *this;
	}
	friend point operator-(point lhs, const point& rhs) {
		return lhs -= rhs;
	}
	point& operator*=(double rhscalar) {
		ptr[0] *= rhscalar;  ptr[1] *= rhscalar;  ptr[2] *= rhscalar;
		return *this;
	}
	friend point operator*(point lhs, double rhscalar) {
		return lhs *= rhscalar;
	}
	point& operator/=(double rhscalar) {
		ptr[0] /= rhscalar;  ptr[1] /= rhscalar;  ptr[2] /= rhscalar;
		return *this;
	}
	friend point operator/(point lhs, double rhscalar) {
		return lhs /= rhscalar;
	}
	friend std::ostream& operator<<(std::ostream& os, const point& obj) {
		return os << "("<<obj.ptr[0]<<","<<obj.ptr[1]<<","<<obj.ptr[2]<<")";
	}
};

/*
	Bonus classes: implementations of point
*/
class TempPoint : public point
{
public:
	double coords[3];
	TempPoint() : point(&(coords[0])) {}
  double & operator [] (int ii) {return coords[ii];}
};
typedef double pts3doubles;


void ShareLocalPositionsWithCacheGroup(std::vector<pts3doubles> * positionsCC, std::vector<pts3doubles> * poscache1) {
  
  MPI_Status recvstatus, sendstatus;
  std::vector<MPI_Request> allSendRequests(mpi_numRanksWithinACache - 1);
  std::vector<MPI_Request> allRecvRequests(mpi_numRanksWithinACache - 1);
  int reqidx=0;
  
  int nextRank = AdvanceRankWithinCache(mpi_myrank);
  int prevRank = ReduceRankWithinCache(mpi_myrank);
  
  for(int ii=0; ii<mpi_numRanksWithinACache; ii++) {
    if(ii == mpi_myIndexWithinCache) {
      memcpy(&((*poscache1)[mpi_myIndexWithinCache*NumLocalParticles*3]), &((*positionsCC)[0]), DBLBYTES*NumLocalParticles*3);
    } else {  
      MPI_Isend(&((*positionsCC)[0]), NumLocalParticles*3, MPI_DOUBLE, nextRank, MPI_MESSAGE_TAG_POSITIONS, MPI_COMM_WORLD, &allSendRequests[reqidx]);
      MPI_Irecv(&((*poscache1)[ii*NumLocalParticles*3]), NumLocalParticles*3, MPI_DOUBLE, prevRank, MPI_MESSAGE_TAG_POSITIONS, MPI_COMM_WORLD, &allRecvRequests[reqidx]);
      nextRank = AdvanceRankWithinCache(nextRank);
      prevRank = ReduceRankWithinCache(prevRank);
      reqidx++;
    }
  }
  MPI_Waitall(reqidx, &(allSendRequests[0]), &sendstatus);
  MPI_Waitall(reqidx, &(allRecvRequests[0]), &recvstatus);
}


void VerletUpdate(
			std::vector<pts3doubles> * positionsAA, // step n-1
			std::vector<pts3doubles> * positionsBB, // step n
			std::vector<pts3doubles> * positionsCC, //predicted step n+1
			std::vector<pts3doubles> * masscache1,
			std::vector<pts3doubles> * masscache2,
			std::vector<pts3doubles> * poscache1,
			std::vector<pts3doubles> * poscache2,
			double dt, double epssqd, bool firstStep)
{
	int pgrp, ii, jj, nextRank, prevRank;
	TempPoint accelSaved;
	TempPoint posdiff;
	double temp;
  const double dtgscalar = (NEWTONS_GRAVITY_CONSTANT * dt * dt);
	std::vector<pts3doubles>* tempptrswap;
	
  
  //initialize to "neighbors"/"buddies" with the same modulo cache-index in the nearby cache
  nextRank = AdvanceRankByCacheAmount(mpi_myrank);
  prevRank = ReduceRankByCacheAmount(mpi_myrank);
  MPI_Status recvstatus, sendstatus;
  MPI_Request sendreqM, recvreqM, sendreqP, recvreqP;
  
	for(pgrp=0; pgrp<mpi_numCacheGroups; pgrp++) {
		
		if(pgrp > 0) { //only send/receive after the first step where everyone calculated within their group
			
      //nonblocking send/receives
			MPI_Isend(&((*masscache1)[0]), masscache1->size(), MPI_DOUBLE, nextRank, MPI_MESSAGE_TAG_POSITIONS, MPI_COMM_WORLD, &sendreqM);
      MPI_Irecv(&((*masscache2)[0]), masscache2->size(), MPI_DOUBLE, prevRank, MPI_MESSAGE_TAG_POSITIONS, MPI_COMM_WORLD, &recvreqM);
      MPI_Isend(&((*poscache1)[0]), poscache1->size(), MPI_DOUBLE, nextRank, MPI_MESSAGE_TAG_POSITIONS, MPI_COMM_WORLD, &sendreqP);
			MPI_Irecv(&((*poscache2)[0]), poscache2->size(), MPI_DOUBLE, prevRank, MPI_MESSAGE_TAG_POSITIONS, MPI_COMM_WORLD, &recvreqP);
      
      MPI_Wait(&recvreqM, &recvstatus);
      MPI_Wait(&sendreqM, &sendstatus);
      MPI_Wait(&recvreqP, &recvstatus);
      MPI_Wait(&sendreqP, &sendstatus);
      
			tempptrswap = poscache1;
			poscache1 = poscache2; //swap, so that next time, what was received will be sent,
			poscache2 = tempptrswap; //so that positions go around the ring
      
      nextRank = AdvanceRankByCacheAmount(nextRank);
      prevRank = ReduceRankByCacheAmount(prevRank);
		}
		
    #ifdef USE_OMP
		//omp_set_num_threads(4);
		#pragma omp parallel shared (masscache1, positionsBB, positionsCC, poscache1, epssqd, NumLocalParticles, NumCachedParticles) private(ii, jj, accelSaved, posdiff, temp)
		#pragma omp for schedule(dynamic) nowait
		#endif
		for(ii=0; ii<NumLocalParticles; ii++) {  //for particles this proc manages
      accelSaved.zero();
			if(pgrp == 0) { //initialize accumulated acceleration to zero at the beginning
        (*positionsCC)[ii*3] = (*positionsCC)[ii*3+1] = (*positionsCC)[ii*3+2] = 0.0;
      }
      for(jj=0; jj<NumCachedParticles; jj++) {  //for other particles (which may or may not include its own group)
				  posdiff[0] = (*poscache1)[jj*3  ] - (*positionsBB)[ii*3  ];
  				posdiff[1] = (*poscache1)[jj*3+1] - (*positionsBB)[ii*3+1];
    			posdiff[2] = (*poscache1)[jj*3+2] - (*positionsBB)[ii*3+2];
					temp = posdiff.length();
					accelSaved += ((posdiff*(*masscache1)[jj]) / (temp*temp*temp + epssqd));
			}
			(*positionsCC)[ii*3  ] += accelSaved[0]; //sum accelerations in positionsCC for now
			(*positionsCC)[ii*3+1] += accelSaved[1]; // accelSaved is lost outside the loop
			(*positionsCC)[ii*3+2] += accelSaved[2];  
		}
	}
  
	//After summing pairwise interactions, finish up (note: the accelerations need to be multiplied by Newtons Gravity Constant; they haven't yet)
	
  if(firstStep) {
    //treat positionsAA as velocity
  	#ifdef USE_OMP
  	//omp_set_num_threads(4);
  	#pragma omp parallel shared (positionsAA, positionsBB, positionsCC, /*dtgscalar,*/ NumLocalParticles) private(ii)
  	#pragma omp for schedule(dynamic) nowait
  	#endif
  	for(ii=0; ii<NumLocalParticles; ii++) {
  		(*positionsCC)[ii*3  ] = (*positionsBB)[ii*3  ] + (*positionsAA)[ii*3  ]*dt + 0.5*(*positionsCC)[ii*3  ]*dtgscalar;
  		(*positionsCC)[ii*3+1] = (*positionsBB)[ii*3+1] + (*positionsAA)[ii*3+1]*dt + 0.5*(*positionsCC)[ii*3+1]*dtgscalar;
    	(*positionsCC)[ii*3+2] = (*positionsBB)[ii*3+2] + (*positionsAA)[ii*3+2]*dt + 0.5*(*positionsCC)[ii*3+2]*dtgscalar;
  	}
  } else {
  	#ifdef USE_OMP
  	//omp_set_num_threads(4);
  	#pragma omp parallel shared (positionsAA, positionsBB, positionsCC, /*dtgscalar,*/ NumLocalParticles) private(ii)
  	#pragma omp for schedule(dynamic) nowait
  	#endif
  	for(ii=0; ii<NumLocalParticles; ii++) {
  		(*positionsCC)[ii*3  ] = (*positionsBB)[ii*3  ]*2.0 - (*positionsAA)[ii*3  ] + (*positionsCC)[ii*3  ] * dtgscalar;
  		(*positionsCC)[ii*3+1] = (*positionsBB)[ii*3+1]*2.0 - (*positionsAA)[ii*3+1] + (*positionsCC)[ii*3+1] * dtgscalar;
    	(*positionsCC)[ii*3+2] = (*positionsBB)[ii*3+2]*2.0 - (*positionsAA)[ii*3+2] + (*positionsCC)[ii*3+2] * dtgscalar;//dtgscalar == NEWTONS_GRAVITY_CONSTANT * dt * dt;
  	}
  }
  
  if(mpi_numCacheGroups > 1) {
    //Now share the cache within the group; this will overwrite poscache1 with the latest positions of the whole cache group
    ShareLocalPositionsWithCacheGroup(positionsCC, poscache1);  
    MPI_Barrier(MPI_COMM_WORLD);
  } else {
    memcpy(&((*poscache1)[0]), &((*positionsCC)[0]), DBLBYTES*NumLocalParticles*3);
  }
  
}



//=====================================================================================================
// "Aarseth" format File I/O


static void readParameters(FILE* inputFile, int* numParticlesPtr, double* timeStepPtr, double* finalTimePtrDbl, double* epsilonSquaredPtr) {
	if(sizeof(double) != DBLBYTES){cout<<"WARNING: unexpected sizeof(double) == "<<sizeof(double)<<endl;}
	double temp;
	cout<<"Initial conditions file header should be: (0)numParticles, (1)eta, (2)timeStep, (3)finalTime, (4)epsilonSquared, (5)gravityconst_G"<<endl;

	//read numparticles
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 0"<<endl; MPIExit(1);}
	(*numParticlesPtr) = RoundDoubleToInt(temp);

	//read eta... not used
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 1"<<endl; MPIExit(1);}

	//read timestep
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 2"<<endl; MPIExit(1);}
	(*timeStepPtr) = temp;

	//read finalTime
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 3"<<endl; MPIExit(1);}
	(*finalTimePtrDbl) = temp;

	//read epsilonSquared
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 4"<<endl; MPIExit(1);}
	(*epsilonSquaredPtr) = temp;

	//read G
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 5"<<endl; MPIExit(1);}
	NEWTONS_GRAVITY_CONSTANT = temp;
}
static void readParticles(FILE* inputFile, std::vector<double> & masses, std::vector<pts3doubles> & positions, std::vector<pts3doubles> & velocities) {
	if(sizeof(double) != DBLBYTES){cout<<"WARNING: unexpected sizeof(double) == "<<sizeof(double)<<endl;}
	double temp;
	int numParticles = masses.size();
	int i, j;
	for(i=0; i<numParticles; i++)
	{
		if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 6 (read "<<i<<" particles before failing)"<<endl; MPIExit(1);}
		masses[i] = temp; assert(isnan(masses[i])==false);
		for(j=0; j<3; j++) {
			if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 7 (read "<<i<<" particles before failing)"<<endl; MPIExit(1);}
			positions[i*3+j] = temp; assert(isnan(positions[i*3+j])==false);
		}
		for(j=0; j<3; j++) {
			if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 8 (read "<<i<<" particles before failing)"<<endl; MPIExit(1);}
			velocities[i*3+j] = temp; assert(isnan(velocities[i*3+j])==false);
		}
    //cout<<"positions["<<i<<"] == ("<<positions[i*3+0]<<", "<<positions[i*3+1]<<", "<<positions[i*3+2]<<")"<<endl;
	}
}
static void writeParticles(FILE* outputFile, const std::vector<pts3doubles> & positions) {
	if(sizeof(float) != FLTBYTES){cout<<"WARNING: unexpected sizeof(float) == "<<sizeof(float)<<endl;}
	float writeme;
	const int npts = (int)positions.size();
	size_t nwritten = 0;
	for(int i=0; i<npts; i++) {
		writeme=(float)positions[i]; nwritten += fwrite(&writeme, FLTBYTES, 1, outputFile);
	}
  if(nwritten < npts){cout<<"Error writing to output file: "<<((int)nwritten)<<" bytes written, expected "<<npts<<endl; MPIExit(1);}
}


//=====================================================================================================
// main()

int main(int argc, char** argv)
{
	std::string fileToOpen;
	std::string fileToSaveTo;
	int nburst; // this is the number of steps before saving to disk
	
	if(argc > 4) {
		mpi_numCacheGroups = atoi(argv[1]);
		nburst = atoi(argv[2]);
		fileToOpen = std::string(argv[3]);
		fileToSaveTo = std::string(argv[4]);
	} else {
		cout<<"Usage:  {NumDataSplits}  {nburst-between-disksaves}  {InitialConditionsFilename-Prefix}  {OutputFilename-Prefix}"<<endl;
    MPIExit(1);
	}
	
  MPI_Status mpi_status;
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &mpi_myrank );
  MPI_Comm_size( MPI_COMM_WORLD, &mpi_size );
	if(mpi_size < mpi_numCacheGroups) {
		cout<<"Error: mpi_size < mpi_numCacheGroups"<<endl;
    MPIExit(1);
	}
	//Calculate cache number, from 0 to (numCaches-1)
  mpi_myCacheNumber = ((mpi_myrank * mpi_numCacheGroups) / mpi_size);
	
	
  //load initial conditions
  
	if(mpi_numCacheGroups > 1) {
		fileToOpen += to_istring(mpi_myCacheNumber); //files should have a numeric index at the end if it was split
	} else {
    fileToOpen += std::string("0");
  }
	fileToOpen += std::string(".data");
  fileToSaveTo += to_istring(mpi_myrank) + std::string(".data");
  
  FILE * inputFile = fopen(fileToOpen.c_str(), "r");
	if(!inputFile) {
		printf("Error: unable to open file: \"%s\"\n",fileToOpen.c_str());
    MPIExit(1);
	}
	double dt, epssqd;
	double finalTimeGiven;
	readParameters(inputFile, &GlobalNumParticles, &dt, &finalTimeGiven, &epssqd);
	dt /= double(nburst);
	int nsteps = RoundDoubleToInt(finalTimeGiven / ((double)dt));
	
  
  //Calculate indices
  
  NumCachedParticles = (GlobalNumParticles / mpi_numCacheGroups);
  NumLocalParticles = (GlobalNumParticles / mpi_size);
  mpi_numRanksWithinACache = (mpi_size / mpi_numCacheGroups);
	
  if(NumLocalParticles*mpi_size != GlobalNumParticles) {
    cout<<"Error: not a divisible number of particles local to an MPI proc"<<endl;
    cout<<"    NumLocalParticles == "<<NumLocalParticles<<", mpi_size == "<<mpi_size<<", GlobalNumParticles == "<<GlobalNumParticles<<", mpi_numCacheGroups == "<<mpi_numCacheGroups<<endl;
    MPIExit(1);
  }
  if(NumCachedParticles*mpi_numCacheGroups != GlobalNumParticles) {
    cout<<"Warning: not a divisible number of cached particles (when partitioning world memory)"<<endl;
    cout<<"    NumLocalParticles == "<<NumLocalParticles<<", mpi_size == "<<mpi_size<<", GlobalNumParticles == "<<GlobalNumParticles<<", mpi_numCacheGroups == "<<mpi_numCacheGroups<<endl;
    MPIExit(1);
  }
  if(mpi_numRanksWithinACache*mpi_numCacheGroups != mpi_size) {
    cout<<"Warning: num MPI procs within a cache group is not a whole number"<<endl;
    cout<<"    NumLocalParticles == "<<NumLocalParticles<<", mpi_size == "<<mpi_size<<", GlobalNumParticles == "<<GlobalNumParticles<<", mpi_numCacheGroups == "<<mpi_numCacheGroups<<endl;
    MPIExit(1);
  }
  
  mpi_myIndexWithinCache = (mpi_myrank % mpi_numRanksWithinACache);
  
  
  cout<<"simulation will use a total of "<<GlobalNumParticles<<" particles and at least "<<(((NumLocalParticles*9+NumCachedParticles*8)*8)/1000)<<" kilobytes of memory"<<endl;
  cout<<endl;
	cout<<"mpi_myrank == "<<mpi_myrank<<", my cache num == "<<mpi_myCacheNumber<<", mpi_myIndexWithinCache ==  "<<mpi_myIndexWithinCache<<endl;
  cout<<"mpi_size == "<<mpi_size<<", NumCachedParticles "<<NumCachedParticles<<" NumLocalParticles "<<NumLocalParticles<<", GlobalNumParticles "<<GlobalNumParticles<<endl;
  cout<<endl;
  
  
  /*
    Allocate lots of memory
  */
	std::vector<pts3doubles> positionsAA(NumLocalParticles*3);
	std::vector<pts3doubles> positionsBB(NumLocalParticles*3);
	std::vector<pts3doubles> positionsCC(NumLocalParticles*3);
	std::vector<double> masscache1(NumCachedParticles);
	std::vector<double> masscache2(NumCachedParticles);
	std::vector<pts3doubles> poscache1(NumCachedParticles*3);
	std::vector<pts3doubles> poscache2(NumCachedParticles*3);
  
  //store this cache's initial positions in "poscache1", velocities in "poscache2"
  readParticles(inputFile, masscache1, poscache1, poscache2);
  
  //copy velocities into AA and positions into BB
  memcpy(&(positionsAA[0]), &(poscache2[mpi_myIndexWithinCache*NumLocalParticles*3]), DBLBYTES*NumLocalParticles*3);
  memcpy(&(positionsBB[0]), &(poscache1[mpi_myIndexWithinCache*NumLocalParticles*3]), DBLBYTES*NumLocalParticles*3);
  
  //we no longer need the info in poscache2 because we've saved local pieces in each node's AA
  
  //do first step, using initial position & velocity, to get to position & position
  VerletUpdate(&positionsAA, &positionsBB, &positionsCC, &masscache1, &masscache2, &poscache1, &poscache2, dt, epssqd, true);
  
  //from here, we just need to continue as if BB is the past step and CC is the current step;
  //and poscache1 contains current step info (all CC's aggregated)
  
	int bidx = 1; //as described just above, we need to go straight to case 1 in the loop below
  
  
	//create output file
	FILE * outputFile = fopen(fileToSaveTo.c_str(), "w");
	if(!outputFile) {
		cout<<"Error: unable to open output file for saving output: \""<<fileToSaveTo<<"\""<<endl;
    MPIExit(1);
	}
	//write output header: first 8 bytes are number of particles
	int64_t NPARTICLESWRITEME = (int64_t)NumLocalParticles;
	fwrite(&NPARTICLESWRITEME, 8, 1, outputFile);
  //next 8 bytes are: 4 for this' MPI proc rank, and 4 for number of MPI procs i.e. number of parallel output files this sim generated
	int32_t NOWRITETHIS = (int32_t)mpi_myrank;
	fwrite(&NOWRITETHIS, 4, 1, outputFile);
  NOWRITETHIS = (int32_t)mpi_size;
	fwrite(&NOWRITETHIS, 4, 1, outputFile);
  
	//write the first two timesteps
	writeParticles(outputFile, positionsBB);
	writeParticles(outputFile, positionsCC);
  
	for(int step=0; step<nsteps; step+=nburst) {
		for(int burst=0; burst<nburst; burst++) {
			switch(bidx) {
				//new will become current and current will become old
				case 0: VerletUpdate(&positionsAA, &positionsBB, &positionsCC, &masscache1, &masscache2, &poscache1, &poscache2, dt, epssqd, false); break;
				case 1: VerletUpdate(&positionsBB, &positionsCC, &positionsAA, &masscache1, &masscache2, &poscache1, &poscache2, dt, epssqd, false); break;
				case 2: VerletUpdate(&positionsCC, &positionsAA, &positionsBB, &masscache1, &masscache2, &poscache1, &poscache2, dt, epssqd, false); break;
			}
			bidx = (bidx+1) % 3;
		}
    
    //save current to disk
		switch(bidx) {
			case 0: writeParticles(outputFile, positionsBB); break;
			case 1: writeParticles(outputFile, positionsCC); break;
			case 2: writeParticles(outputFile, positionsAA); break;
		}
    
		cout<<"simulation time elapsed: "<<(dt*((float)nburst)*((float)(1+(step/nburst))))<<endl;
	}

	fclose(outputFile);
	fclose(inputFile);

  MPI_Finalize();
	return 0;
}
