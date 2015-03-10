/*
   nbodycpp.cpp
   Very small and lightweight O(N^2) nbody integrator...
      can be used for benchmarking against OpenCL
   
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <iostream>
#include <vector>
using std::cout; using std::endl;


static double NEWTONS_GRAVITY_CONSTANT = 1.0;


//=====================================================================================================
// Miscellaneous

int RoundDoubleToInt(double x) {
	return static_cast<int>((x>=0.0) ? floor(x+0.5) : ceil(x-0.5));
}

class point {
public:
	double x,y,z;
	point() : x(0.0),y(0.0),z(0.0) {}
	point(double X, double Y, double Z) : x(X),y(Y),z(Z) {}
	
	double length() const {
		return sqrt(x*x + y*y + z*z);
	}
	double dotwith(const point& other) {
		return x*other.x + y*other.y + z*other.z;
	}
	double & at(int idx) {return (idx == 0 ? x : (idx == 1 ? y : z));}
	void zero() {x=0.0; y=0.0; z=0.0;}
	
	
	point& operator+=(const point& rhs) {
		x += rhs.x;  y += rhs.y;  z += rhs.z;
		return *this;
	}
	friend point operator+(point lhs, const point& rhs) {
		return lhs += rhs;
	}
	point& operator-=(const point& rhs) {
		x -= rhs.x;  y -= rhs.y;  z -= rhs.z;
		return *this;
	}
	friend point operator-(point lhs, const point& rhs) {
		return lhs -= rhs;
	}
	point& operator*=(double rhscalar) {
		x *= rhscalar;  y *= rhscalar;  z *= rhscalar;
		return *this;
	}
	friend point operator*(point lhs, double rhscalar) {
		return lhs *= rhscalar;
	}
	point& operator/=(double rhscalar) {
		x /= rhscalar;  y /= rhscalar;  z /= rhscalar;
		return *this;
	}
	friend point operator/(point lhs, double rhscalar) {
		return lhs /= rhscalar;
	}
	friend std::ostream& operator<<(std::ostream& os, const point& obj) {
		return os << "("<<obj.x<<","<<obj.y<<","<<obj.z<<")";
	}
};


void VerletUpdate(std::vector<double> & masses,
			std::vector<point> & positionsAA, // step n-1
			std::vector<point> & positionsBB, // step n
			std::vector<point> & positionsCC, //predicted step n+1
			double dt, double epssqd)
{
	int nparticles = masses.size();
	point accelSaved;
	point posdiff;
	double temp;
	for(int i=0; i<nparticles; i++) {
		accelSaved.zero();
		for(int j=0; j<nparticles; j++) {
			if(i != j) {
				posdiff = positionsBB[j] - positionsBB[i];
				temp = posdiff.length();
				accelSaved += ((posdiff*NEWTONS_GRAVITY_CONSTANT*masses[j]) / (temp*temp*temp + epssqd));
			}
		}
		positionsCC[i] = positionsBB[i]*2.0 - positionsAA[i] + accelSaved*dt*dt;
	}
}


//=====================================================================================================
// Aarseth File I/O

#define DBLBYTES 8
#define FLTBYTES 4

static void readParameters(FILE* inputFile, int* numParticlesPtr, double* timeStepPtr, double* finalTimePtrDbl, double* epsilonSquaredPtr) {
	if(sizeof(double) != DBLBYTES){cout<<"WARNING: unexpected sizeof(double) == "<<sizeof(double)<<endl;}
	double temp;
	cout<<"Initial conditions file header should be: (0)numParticles, (1)eta, (2)timeStep, (3)finalTime, (4)epsilonSquared, (5)gravityconst_G"<<endl;
	
	//read numparticles
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 0"<<endl; exit(1);}
	(*numParticlesPtr) = RoundDoubleToInt(temp);
	
	//read eta... not used
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 1"<<endl; exit(1);}
	
	//read timestep
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 2"<<endl; exit(1);}
	(*timeStepPtr) = temp;
	
	//read finalTime
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 3"<<endl; exit(1);}
	(*finalTimePtrDbl) = temp;
	
	//read epsilonSquared
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 4"<<endl; exit(1);}
	(*epsilonSquaredPtr) = temp;
	
	//read G
	if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 5"<<endl; exit(1);}
	NEWTONS_GRAVITY_CONSTANT = temp;
}
static void readParticles(FILE* inputFile, std::vector<double> & masses, std::vector<point> & positions, std::vector<point> & velocities) {
	if(sizeof(double) != DBLBYTES){cout<<"WARNING: unexpected sizeof(double) == "<<sizeof(double)<<endl;}
	double temp;
	int numParticles = masses.size();
	int i, j;
	for(i=0; i<numParticles; i++)
	{
		if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 6 (read "<<numParticles<<" particles before failing)"<<endl; exit(1);}
		masses[i] = temp; assert(isnan(masses[i])==false);
		for(j=0; j<3; j++) {
			if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 7 (read "<<numParticles<<" particles before failing)"<<endl; exit(1);}
			positions[i].at(j) = temp; assert(isnan(positions[i].at(j))==false);
		}
		for(j=0; j<3; j++) {
			if(fread(&temp, DBLBYTES, 1, inputFile) < 1){cout<<"Error reading initial conditions file 8 (read "<<numParticles<<" particles before failing)"<<endl; exit(1);}
			velocities[i].at(j) = temp; assert(isnan(velocities[i].at(j))==false);
		}
	}
}
static void writeParticles(FILE* outputFile, const std::vector<point> & positions) {
	if(sizeof(float) != FLTBYTES){cout<<"WARNING: unexpected sizeof(float) == "<<sizeof(float)<<endl;}
	float writeme;
	int nparticles = positions.size();
	size_t nwritten;
	for(int i=0; i<nparticles; i++) {
		writeme=(float)positions[i].x;   nwritten  = fwrite(&writeme, FLTBYTES, 1, outputFile);
		writeme=(float)positions[i].y;   nwritten += fwrite(&writeme, FLTBYTES, 1, outputFile);
		writeme=(float)positions[i].z;   nwritten += fwrite(&writeme, FLTBYTES, 1, outputFile);
		if(nwritten < 3){cout<<"Error writing to output file"<<endl; exit(1);}
	}
}


//=====================================================================================================
// main()

int main(int argc, char** argv)
{
	char fileToOpen[1024];
	char fileToSaveTo[1024];
	memset(fileToOpen,0,1024);
	if(argc >= 2) {
		strcpy(fileToOpen, argv[1]);
	} else {
		cout<<"Usage:  [InitialConditionsFilename]"<<endl;
		return 1;
	}
	FILE * inputFile = fopen(fileToOpen, "r");
	if(!inputFile) {
		printf("Error: unable to open file: \"%s\"\n",fileToOpen);
		return 1;
	}
	
	int nburst = 6;  // this is the number of steps before saving to disk
			 // should divide the value of nstep without remainder...
			 // MUST be divisible by three
	
	int nparticles;
	double dt, epssqd;
	double finalTimeGiven;
	readParameters(inputFile, &nparticles, &dt, &finalTimeGiven, &epssqd);
	dt /= double(nburst);
	int nsteps = RoundDoubleToInt(finalTimeGiven / ((double)dt));
	
	cout<<"simulation will use "<<nparticles<<" particles"<<endl;
	
	std::vector<double> masses(nparticles);
	std::vector<point> velocities(nparticles);
	std::vector<point> positionsAA(nparticles);
	std::vector<point> positionsBB(nparticles);
	std::vector<point> positionsCC(nparticles);
	
	readParticles(inputFile, masses, positionsAA, velocities);
	
	point accelSaved;
	point posdiff;
	double temp;
	for(int i=0; i<nparticles; i++) {
		accelSaved.zero();
		for(int j=0; j<nparticles; j++) {
			if(i != j) {
				posdiff = positionsBB[j] - positionsBB[i];
				temp = posdiff.length();
				accelSaved += ((posdiff*NEWTONS_GRAVITY_CONSTANT*masses[j]) / (temp*temp*temp + epssqd));
			}
		}
		positionsBB[i] = positionsAA[i] + velocities[i]*dt + accelSaved*0.5*dt*dt;
	}
	
	sprintf(fileToSaveTo, "out_simplecpu.data");
	FILE * outputFile = fopen(fileToSaveTo, "w");
	writeParticles(outputFile, positionsAA);
	writeParticles(outputFile, positionsBB);
	
	for(int step=0; step<nsteps; step+=nburst) {
		for(int burst=0; burst<nburst; burst+=3) {
			VerletUpdate(masses, positionsAA, positionsBB, positionsCC, dt, epssqd);
			VerletUpdate(masses, positionsBB, positionsCC, positionsAA, dt, epssqd);
			VerletUpdate(masses, positionsCC, positionsAA, positionsBB, dt, epssqd);
		}
		//save to disk
		writeParticles(outputFile, positionsBB);
		
		//cout<<"so far, completed "<<(1+(step/nburst))<<" bursts"<<endl;
		cout<<"simulation time elapsed: "<<(dt*((float)nburst)*((float)(1+(step/nburst))))<<endl;
	}
	
	fclose(outputFile);
	fclose(inputFile);
	return 0;
}


