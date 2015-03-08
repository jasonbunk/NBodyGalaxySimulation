/*
 * Interactivity handled as a kind of "game system".
 *
 * Author: Jason Bunk
 * Web page: http://sites.google.com/site/jasonbunk
 * License: Apache License Version 2.0, January 2004
 * Copyright (c) 2015 Jason Bunk
 */

#include "stdafx.h"
#include "GameSystemBase.h"
#include "SimulationSystem.h"
#include <iostream>
#ifndef memcpy
#include <cstring>
#endif
using std::cout; using std::endl;

#define TIMESCALING_SCALES_GAMETIME 1


/*extern*/ GameSystemBase gGameSystem = GameSystemBase(); //extern from "stdafx.h"


GameSystemBase::GameSystemBase(const GameSystemBase & rhs) :
	initialized(rhs.initialized),
	m_currFrameTime(rhs.m_currFrameTime),
	m_TotalSimulatedTimeElapsed(rhs.m_TotalSimulatedTimeElapsed),
	maximum_time_step(rhs.maximum_time_step),
	fixed_time_step(rhs.fixed_time_step),
	fixed_time_step_accumulator(rhs.fixed_time_step_accumulator),
	fixed_timestep_randomizer__device(),
	fixed_timestep_randomizer__stddev(0.01),
	m_avg_totalrecentFrameTimes(rhs.m_avg_totalrecentFrameTimes),
	m_avgFrame_oldest(rhs.m_avgFrame_oldest),
	MouseDown_L(rhs.MouseDown_L),
	MouseDown_R(rhs.MouseDown_R),
	ResizeWindowFunc(rhs.ResizeWindowFunc),
	always_use_gamepads(rhs.always_use_gamepads),
//	mouse_pointer_tex_base_shared(rhs.mouse_pointer_tex_base_shared),
//	mouse_pointer_tex(rhs.mouse_pointer_tex),
	physics_system(rhs.physics_system),
	camera_original_r(rhs.camera_original_r)
{
    memcpy(m_avgFrameTimes, rhs.m_avgFrameTimes, NUMBER_OF__AVG_FRAMES_TO_COUNT);
}


GameSystemBase::GameSystemBase() :
initialized(false),
MouseDown_L(false),
MouseDown_R(false),
m_currFrameTime(1.0),
m_TotalSimulatedTimeElapsed(0.0),
maximum_time_step(0.25),
fixed_time_step(0.1*(1.0/60.0)),
fixed_timestep_randomizer__device(),
fixed_timestep_randomizer__stddev(0.01),
fixed_time_step_accumulator(0.0),
m_avg_totalrecentFrameTimes(0.0),
m_avgFrame_oldest(0), //which frame is the oldest? which one to overwrite?
always_use_gamepads(false),
ResizeWindowFunc(nullptr),
physics_system(nullptr),
camera_original_r(1.0)
{
	for(int a=0; a<NUMBER_OF__AVG_FRAMES_TO_COUNT; a++) //keeps track of the last bunch of frames
		m_avgFrameTimes[a] = 0.0;
}

GameSystemBase::~GameSystemBase()
{
	if(physics_system != nullptr)
	{
		delete physics_system;
		physics_system = nullptr;
	}
}



void GameSystemBase::SetMousePos(const phys::point & true_coords, const phys::point& newPos)
{
	true_mouse_pos_win_coords = true_coords;
	mouse_pos = newPos;
}



void GameSystemBase::Draw_MousePointer()
{
}

void GameSystemBase::Draw_EverythingExceptMousePointer()
{
	if(physics_system != nullptr) {
		physics_system->draw();
	} else {
        std::cout<<"Warning: no system to draw?"<<std::endl;
	}
}

void GameSystemBase::Update(double frameTime)
{
	m_currFrameTime = frameTime;

	//---------------------------------------------------------------
	m_avgFrameTimes[m_avgFrame_oldest] = frameTime;

	m_avg_totalrecentFrameTimes = 0.0;
	for(int ii=0; ii<NUMBER_OF__AVG_FRAMES_TO_COUNT; ii++) {
		m_avg_totalrecentFrameTimes += m_avgFrameTimes[ii];
	}
	m_avgFrame_oldest++;
	if(m_avgFrame_oldest >= NUMBER_OF__AVG_FRAMES_TO_COUNT)
		m_avgFrame_oldest = 0;
	//---------------------------------------------------------------
	
#if TIMESCALING_SCALES_GAMETIME
#else
	if(cheats.timescale != 1.0)
	{
		if(cheats.timescale > 1e-10)
			m_currFrameTime *= cheats.timescale;
		else
			m_currFrameTime *= 1e-10; //don't let time steps go to zero
	}
	frameTime = m_currFrameTime;
#endif

	if(fixed_time_step > 0.0)
	{
		fixed_time_step_accumulator += frameTime;
		
		if(fixed_time_step_accumulator > 0.4) {
			cout<<"accumulated too large of a time step: "<<fixed_time_step_accumulator<<" ... rolling back to limit 0.4 sec"<<endl;
			fixed_time_step_accumulator = 0.4;
		}
		
		while(fixed_time_step_accumulator >= fixed_time_step)
		{
			double this_step_size = fixed_time_step;
			
			if(fixed_timestep_randomizer__stddev > 0.0) {
				this_step_size = fixed_time_step + normal_dist_box_muller(&fixed_timestep_randomizer__device, 0.0, fixed_timestep_randomizer__stddev);
				if(this_step_size > maximum_time_step) {
					this_step_size = maximum_time_step;
				} else if(this_step_size < 0.00000001) {
					this_step_size = 0.00000001;
				}
			}
			fixed_time_step_accumulator -= this_step_size;
			frameTime = m_currFrameTime = this_step_size;
			//-------------------------------------------
#if TIMESCALING_SCALES_GAMETIME
			if(cheats.timescale != 1.0)
			{
				if(cheats.timescale > 1e-10)
					m_currFrameTime *= cheats.timescale;
				else
					m_currFrameTime *= 1e-10; //don't let time steps go to zero
			}
#endif
			this->DoUpdateStuff(frameTime);
			//-------------------------------------------
		}
	}
	else
	{
#if TIMESCALING_SCALES_GAMETIME
		if(cheats.timescale != 1.0)
		{
			if(cheats.timescale > 1e-10)
				m_currFrameTime *= cheats.timescale;
			else
				m_currFrameTime *= 1e-10; //don't let time steps go to zero
		}
#endif
		this->DoUpdateStuff(frameTime);
	}
}

/*virtual*/ void GameSystemBase::DoUpdateStuff(double frametime_before_cheatscaled)
{
//---------------
	//camera motion is independent of "cheat" time scaling

	if(GetKeyboard()->keyboard['w'])
	{
		camera_pos.x -= (0.4 * camera_rotation.r*sin(camera_rotation.phi) * frametime_before_cheatscaled);
		camera_pos.y -= (0.4 * camera_rotation.r*cos(camera_rotation.phi) * frametime_before_cheatscaled);
	}
	if(GetKeyboard()->keyboard['s'])
	{
		camera_pos.x += (0.4 * camera_rotation.r*sin(camera_rotation.phi) * frametime_before_cheatscaled);
		camera_pos.y += (0.4 * camera_rotation.r*cos(camera_rotation.phi) * frametime_before_cheatscaled);
	}

	if(GetKeyboard()->keyboard['a'])
	{
		camera_pos.x += (0.4 * camera_rotation.r*cos(camera_rotation.phi) * frametime_before_cheatscaled);
		camera_pos.y -= (0.4 * camera_rotation.r*sin(camera_rotation.phi) * frametime_before_cheatscaled);
	}
	if(GetKeyboard()->keyboard['d'])
	{
		camera_pos.x -= (0.4 * camera_rotation.r*cos(camera_rotation.phi) * frametime_before_cheatscaled);
		camera_pos.y += (0.4 * camera_rotation.r*sin(camera_rotation.phi) * frametime_before_cheatscaled);
	}

	if(GetKeyboard()->keyboard[45]) //minus
		camera_rotation.r *= (1.0 + (1.5*frametime_before_cheatscaled));

	if(GetKeyboard()->keyboard[61]) //equals plus
		camera_rotation.r /= (1.0 + (1.5*frametime_before_cheatscaled));


//-------------
	//using "cheat" time scaling

#if TIME_GOES_BACKWARDS
	m_currFrameTime *= -1.0;
#endif
	
	const bool doOnceUpdatesFirst = true; //ONLY FOR KALMAN_SPRINGS_SIM
	
	if(physics_system != nullptr && SystemIsPaused == false)
	{
		m_TotalSimulatedTimeElapsed += m_currFrameTime;
		
		if(doOnceUpdatesFirst) {
			physics_system->RespondToKeyStates();
			physics_system->UpdateSystemStuff_OncePerFrame(m_currFrameTime);
		}
		
		physics_system->UpdateSystemStuff_EachPhysicsStep(m_currFrameTime * 0.02);
		
		if(doOnceUpdatesFirst==false) {
			physics_system->RespondToKeyStates();
			physics_system->UpdateSystemStuff_OncePerFrame(m_currFrameTime);
		}
	}
}


void GameSystemBase::Init()	//float windWidth, float windHeight, float glUnitsX, float glUnitsY)
{
	cout<<"-------------- GameSystemBase initialized"<<endl;
	initialized = true;
}


void GameSystemBase::SetMouseL(bool down)
{
	if(down != MouseDown_L && down)
	{
		//just was clicked; do stuff

		if(physics_system != nullptr)
			physics_system->OnMouseLdown();
	}
	MouseDown_L = down;
}
void GameSystemBase::SetMouseR(bool down)
{
	MouseDown_R = down;
}

KeyBoardClass* GameSystemBase::GetKeyboard()
{
	if(keyboardclass == nullptr)
	{
		keyboardclass = new KeyBoardClass();
	}

	return keyboardclass;
}

void KeyBoardClass::OnKeyPressed(char the_key)
{
	if(the_key == 32) //spacebar
		SystemIsPaused = !SystemIsPaused;
}
