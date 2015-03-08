
#ifndef __GAME_SYSTEM_BASE_H__
#define __GAME_SYSTEM_BASE_H__

#define NOMINMAX

#include <string>
#include "TryIncludeJPhysics.h"
#include "Utils/random_normal_dist_using_c.h"


#define NUMBER_OF__AVG_FRAMES_TO_COUNT 11
#define NUMBER_OF__AVG_FRAMES_TO_COUNT_F 11.0f
#define NUMBER_OF__AVG_FRAMES_TO_COUNT_D 11.0



#define KEYBOARDINPUT_SHIFTKEY_ASCII 7 //just a useless bell icon anyway, it's not even on keyboards

class KeyBoardClass
{
public:

#if BUILD_WITH_SFML
	sf::Uint32 last_unicode_key_entered;
#endif

	bool keyboard[256];
	

	virtual void OnKeyPressed(char the_key);


	KeyBoardClass() : last_unicode_key_entered(0)
	{
		std::fill(keyboard, keyboard+256, false);
	}
};


class Cheats
{
public:
	double timescale;
	bool do_largetimestep_remainders;
	bool extradebuginfo;

	Cheats() :
	timescale(1.0),
	do_largetimestep_remainders(true),
	extradebuginfo(false)
	{}
};


class SimulationSystem;


class GameSystemBase
{
public:
	double fixed_time_step;
	double fixed_timestep_randomizer__stddev;
	
protected:
	bool initialized;

	double m_currFrameTime;
	double m_TotalSimulatedTimeElapsed;

	double maximum_time_step;
	
	double fixed_time_step_accumulator;
	RNG_rand_r fixed_timestep_randomizer__device;
	


	double m_avgFrameTimes[NUMBER_OF__AVG_FRAMES_TO_COUNT]; //keeps track of the last bunch of frames
	double m_avg_totalrecentFrameTimes; //subtract the oldest frame from this, and add the newest frame
	unsigned char m_avgFrame_oldest; //which frame is the oldest? which one to overwrite next frame?


	bool MouseDown_L;
	bool MouseDown_R;

	
	KeyBoardClass* keyboardclass;

	
	//SharedTexture mouse_pointer_tex_base_shared;
	//SharedTextureInstance_NotAnimated mouse_pointer_tex;

public:
	Cheats cheats;

	SimulationSystem* physics_system;


//----------
	phys::vec3 camera_pos;

	double camera_original_r;

	phys::vec3_spherical camera_rotation; //around the point that the camera is focusing at (camera_pos)
//----------



	inline double FrameTime()			{return m_currFrameTime;}
	inline double GetAvgFrameTime()		{return m_avg_totalrecentFrameTimes;}
	inline double GetAvgFrameRate()		{return (physmath::cmpDoubleToZero(m_avg_totalrecentFrameTimes) ? m_avg_totalrecentFrameTimes : (1.0 / m_avg_totalrecentFrameTimes));}
	
	inline void SetMaximumTimeStep(double newstep) {if(newstep > 1.0e-13){maximum_time_step = newstep;}}

	inline double GetTimeElapsed()	{return m_TotalSimulatedTimeElapsed;}
	inline void SetTimeElapsed(double newtime) {m_TotalSimulatedTimeElapsed = newtime;}

	inline bool IsInitialized() {return initialized;}

	KeyBoardClass* GetKeyboard();


	inline bool GetMouseDown_L() {return MouseDown_L;}
	inline bool GetMouseDown_R() {return MouseDown_R;}
	void SetMouseL(bool down);
	void SetMouseR(bool down);
	void CheckClick();




	void (*ResizeWindowFunc)(int,int);


	bool always_use_gamepads;
	phys::point mouse_pos;
	phys::point true_mouse_pos_win_coords;
	
	void SetMousePos(const phys::point & true_coords, const phys::point& newPos);
	inline const phys::point& GetMousePos_GLcoords() {return mouse_pos;}
	inline const phys::point& GetMousePos_Wincoords() {return true_mouse_pos_win_coords;}


	//void ResizeWindow(float windWidth, float windHeight, float glUnitsX, float glUnitsY);
	void Init();//float windWidth, float windHeight, float glUnitsX, float glUnitsY);

	void Update(double frameTime);

	void Draw_MousePointer();
	void Draw_EverythingExceptMousePointer();

	GameSystemBase();
	GameSystemBase(const GameSystemBase & rhs);
	~GameSystemBase();

protected:

	virtual void DoUpdateStuff(double frametime_before_cheatscaled);
};

#endif
