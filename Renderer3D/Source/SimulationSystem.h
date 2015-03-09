#ifndef ___SIMPLER_GAME_SIM_SYSTEM_SIM__H___
#define ___SIMPLER_GAME_SIM_SYSTEM_SIM__H___

#include "TryIncludeJPhysics.h"


class SimulationSystem
{
public:
	virtual void draw() = 0;
	
	virtual void InitBeforeSimStart() = 0;

	virtual bool DoFixedTimeSteps() {return false;}
	virtual double GetFixedTimeStep_IfNeeded() {return 0.1;}
	virtual int NumFixedTimeStepsPerFrame_IfNeeded() {return 1;}
	virtual int GetNumFramesDraw() {return -1;}
	
	virtual void OnMouseLdown() {}
	virtual void RespondToKeyStates() {}

	virtual phys::vec3 GetOriginForDrawingPlanes() {return phys::vec3();}

	virtual void SetCameraPosGameOffset(phys::vec3 & vec_to_update) {vec_to_update.Nullify();}

	virtual double GetMinDist_ForCameraClipping() const {return 0.001;}
	virtual double GetMaxDist_ForCameraClipping() const {return 10000.0;}
	virtual double GetGridWidth_ForDrawingPlanes() const {return 1.0;}

	virtual void UpdateSystemStuff_EachPhysicsStep(double frametime) {}
	virtual void UpdateSystemStuff_OncePerFrame(double frametime) {}
	virtual void DrawSystemStuff() {}
	virtual bool FormatTextInfo(char* text_buffer, int line_n) {return false;}
};

#endif
