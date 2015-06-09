#ifndef ___WINDOW_PROJECTION_INFO_H____
#define ___WINDOW_PROJECTION_INFO_H____


class WindowProjectionInfo
{
public:
	int WindowHeight;
	int WindowWidth;
	float WindowHeight_half_f;
	float WindowWidth_half_f;

	GLdouble last_used_modelMatrix[16];
	GLdouble last_used_projMatrix[16];
	GLint last_used_viewport[4];

	WindowProjectionInfo() : WindowHeight(0), WindowWidth(0), WindowHeight_half_f(0.0f), WindowWidth_half_f(0.0f) {}
};


#endif