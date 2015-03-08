/*
 * Render text onscreen.
 *
 * Author: Jason Bunk
 * Web page: http://sites.google.com/site/jasonbunk
 * License: Apache License Version 2.0, January 2004
 * Copyright (c) 2015 Jason Bunk
 */
#include "stdafx.h"
#include "GlutFontRender.h"
#include "TryIncludeJPhysics.h"
#include "IncludeGameEngine.h"
#include "main.h"



/*extern*/ sf::RenderWindow* sfml_window = nullptr;
/*extern*/ sf::Font font_for_debugstuff = sf::Font();



void renderBitmapString(
		float posx,
		float posy,
		const char *string,
		float charheight/*=18.0f*/,
		float* returned_width_of_printed_pixels/*=NULL*/)
{
	if(returned_width_of_printed_pixels != NULL)
		*returned_width_of_printed_pixels = 0.0f;

	sf::Text text(string, font_for_debugstuff);
	text.setColor(sf::Color(255, 255, 255, 255));

	float pixelheight = physmath::RoundFloatToInteger(charheight);
	text.setCharacterSize(physmath::RoundFloatToInt(pixelheight));

	text.setPosition(	posx + global_window_info.WindowWidth_half_f,
						(global_window_info.WindowHeight_half_f * 2.0f) - (pixelheight + posy + global_window_info.WindowHeight_half_f));

	// Draw some text on top of our OpenGL object
	sfml_window->pushGLStates();
	sfml_window->draw(text);
	sfml_window->popGLStates();
}
