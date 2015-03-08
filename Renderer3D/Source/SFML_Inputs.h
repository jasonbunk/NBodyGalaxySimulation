#ifndef __SFML_INPUTS___HPP____
#define __SFML_INPUTS___HPP____


#define SET_GAMESYSTEMS_KEYBOARDKEY_NEW_STATE(keyyy) \
{if(isdown && gGameSystem.GetKeyboard()->keyboard[keyyy] == false){gGameSystem.GetKeyboard()->OnKeyPressed(keyyy);} gGameSystem.GetKeyboard()->keyboard[keyyy] = isdown;}


#if BUILD_WITH_SFML
void key_up_or_down_func(sf::Keyboard::Key key, bool isdown)
{
	if(key >= sf::Keyboard::Num0 && key <= sf::Keyboard::Num9)
	{
		SET_GAMESYSTEMS_KEYBOARDKEY_NEW_STATE(key+22);
	}
	else if(key >= sf::Keyboard::A && key <= sf::Keyboard::Z)
	{
		SET_GAMESYSTEMS_KEYBOARDKEY_NEW_STATE(key+97); //always is lowercase
	}
	else
	{
		switch(key)
		{
		case sf::Keyboard::Escape:
			SET_GAMESYSTEMS_KEYBOARDKEY_NEW_STATE(27); break;
		case sf::Keyboard::LShift:
			SET_GAMESYSTEMS_KEYBOARDKEY_NEW_STATE(KEYBOARDINPUT_SHIFTKEY_ASCII); break;
		case sf::Keyboard::Space:
			SET_GAMESYSTEMS_KEYBOARDKEY_NEW_STATE(32); break;
		case sf::Keyboard::BackSpace:
		case sf::Keyboard::Delete:
			SET_GAMESYSTEMS_KEYBOARDKEY_NEW_STATE(8); break;
		case sf::Keyboard::Comma:
			SET_GAMESYSTEMS_KEYBOARDKEY_NEW_STATE(44); break;
		case sf::Keyboard::Dash:
			SET_GAMESYSTEMS_KEYBOARDKEY_NEW_STATE(45); break;
		case sf::Keyboard::Period:
			SET_GAMESYSTEMS_KEYBOARDKEY_NEW_STATE(46); break;
		case sf::Keyboard::Slash:
			SET_GAMESYSTEMS_KEYBOARDKEY_NEW_STATE(47); break;
		case sf::Keyboard::SemiColon:
			SET_GAMESYSTEMS_KEYBOARDKEY_NEW_STATE(59); break;
		case sf::Keyboard::Equal:
			SET_GAMESYSTEMS_KEYBOARDKEY_NEW_STATE(61); break;
		}
	}

	//"cheats"

	if(isdown)
	{
		switch(key)
		{
		case sf::Keyboard::LBracket:
			gGameSystem.cheats.timescale *= 0.6;
			break;
		case sf::Keyboard::RBracket:
			gGameSystem.cheats.timescale /= 0.6;
			break;
		case sf::Keyboard::Period:
			gGameSystem.cheats.timescale = 1.0e-9;
			break;
		case sf::Keyboard::Slash:
			gGameSystem.cheats.timescale = 1.0;
			break;
		}
	}
}
#endif


#endif
