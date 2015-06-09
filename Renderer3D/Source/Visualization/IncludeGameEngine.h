#ifndef ____INCLUDE_GAMEENGINE___H_____
#define ____INCLUDE_GAMEENGINE___H_____

//building with SFML

#define BUILD_WITH_SFML 1

#ifdef BUILD_WITH_SFML
#include <SFML/Graphics.hpp>

extern sf::RenderWindow* sfml_window; //defined in "GlutFontRender.cpp"
extern sf::Font font_for_debugstuff;
#endif

#endif
