#ifndef ___GLUT_FONT_RENDER_H____
#define ___GLUT_FONT_RENDER_H____


//defined in GlutFontRender.cpp

void renderBitmapString(
		float posx,
		float posy,
		const char *string,
		float charheight=18.0f,
		float* returned_width_of_printed_pixels=NULL);


#endif
