/*
 * Initialize simulation renderer.
 *
 * Author: Jason Bunk
 * Web page: http://sites.google.com/site/jasonbunk
 * 
 * Copyright (c) 2015 Jason Bunk
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
#include "stdafx.h"
#include "InitSimulations.h"
#include "SimulationOutputRenderer.h"
#include "main.h"
#include <iostream>
using std::cout; using std::endl;


void StartSimulation(int argc, char** argv)
{
	if(argc < 4) {
		cout<<"Usage:  [DATAFILE-TO-RENDER]  [TWOCOLOR?]  [AUTOSAVEIMAGES?]  [optional:showrenderfps?]  [optional:showgrid?]  [optional:camera-coords-file]  [optional:autoplay]"<<endl;
		exit(1);
	}
	SimulationOutputRenderer * simsys = new SimulationOutputRenderer();
	simsys->OpenDataFile(argv[1]);
	simsys->drawtwocolors = (atoi(argv[2]) != 0);
	
	std::string argv2(argv[3]);
	if(argv2 == "1" || argv2 == "TRUE" || argv2 == "True" || argv2 == "true") {
		cout<<"will autosave images!"<<endl;
		AutoSaveRenderedImages = true;
		SystemIsPaused = false;
	} else {
		cout<<"wont autosave images, will be interactive (press space to unpause)"<<endl;
		AutoSaveRenderedImages = false;
		SystemIsPaused = true;
	}
	
	if(argc >= 5) {
		std::string argv2(argv[4]);
		if(argv2 == "1" || argv2 == "TRUE" || argv2 == "True" || argv2 == "true") {
			cout<<"will show debug render text (e.g. fps)..."<<endl;
			DisplayDebugRenderText = true;
		} else {
			cout<<"hiding debug render text (e.g. fps)..."<<endl;
			DisplayDebugRenderText = false;
		}
	}
	
	if(argc >= 6) {
		std::string argv2(argv[5]);
		if(argv2 == "1" || argv2 == "TRUE" || argv2 == "True" || argv2 == "true") {
			cout<<"will show reference grid..."<<endl;
			DrawGridForScaleReference = true;
		} else {
			cout<<"hiding reference grid..."<<endl;
			DrawGridForScaleReference = false;
		}
	}
	
	gGameSystem.camera_rotation.Nullify();
	gGameSystem.camera_pos.Nullify();
	
    gGameSystem.camera_rotation.r = 10.0;
    gGameSystem.camera_original_r = 10.0;
	if(argc >= 7) {
		std::ifstream cameraposfile(argv[6]);
		bool success = false;
		if(cameraposfile.is_open() && cameraposfile.good()) {
			if(cameraposfile >> gGameSystem.camera_pos.x >> gGameSystem.camera_pos.y >> gGameSystem.camera_pos.z
						>> gGameSystem.camera_rotation.r >> gGameSystem.camera_rotation.phi >> gGameSystem.camera_rotation.theta) {
					success = true;
					cout<<"successfully read camera position file!!"<<endl;
			}
		}
		if(success == false) {
			cout<<"ERROR reading camera position file!"<<endl;
		}
		cameraposfile.close();
	}
	
	if(argc >= 8) {
		std::string argv2(argv[7]);
		if(argv2 == "1" || argv2 == "TRUE" || argv2 == "True" || argv2 == "true") {
			cout<<"will autoplay at full speed..."<<endl;
			SystemIsPaused = false;
			gGameSystem.cheats.timescale = 1000.0; //as fast as it can read the files
		} else {
			cout<<"wont (will NOT) autoplay at full speed"<<endl;
			SystemIsPaused = true;
		}
	}
	
	gGameSystem.cheats.do_largetimestep_remainders = false;
	simsys->InitBeforeSimStart();

	if(gGameSystem.physics_system != nullptr) {
		delete gGameSystem.physics_system;
	}
	gGameSystem.physics_system = simsys;
}






