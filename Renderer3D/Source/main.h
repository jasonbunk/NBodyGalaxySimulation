#ifndef ___MAIN_H_____
#define ___MAIN_H_____


// Function prototypes

void myMouseFunc( int button, int state, int x, int y );
void myMouseMotionFunc( int x, int y );

void updatePhysics(void);
void displayFunc(void);

void setupOpenGLSmoothRendering();
void resizeWindow(int w, int h);


void StartSimulation(int argc, char** argv);


#define DEFAULT_CAMERA_POS_Z -500.0
#include "Visualization/WindowProjectionInfo.h"
extern WindowProjectionInfo global_window_info; //defined in main.cpp


extern bool DisplayDebugRenderText;
extern bool DrawGridForScaleReference;
extern bool AutoSaveRenderedImages;


#endif
