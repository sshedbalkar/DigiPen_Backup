#include "Precompiled.h"
#include "Global.h"
#include "ReturnCodes.h"
#include "TimeCalculator.h"
#include "TextDisplay.h"
#include "Menu.h"
#include "TextureDraw.h"
#include "MainMenu.h"
#include "GameHistory.h"
#include "MemAllocatorGen.h"
#include "OptionsMenu.h"
//
namespace Framework
{
	utility::TimeCalculator*		g_timeCalculator;
	TextDisplay*					g_textDisplay;
	Menu*							g_menu;
	TextureDraw*					g_textureDraw;
	MainMenu*						g_mainMenu;
	GameHistory*					g_gameHistory;
	Memory::MemAllocatorGen*		g_memAllocator;
	OptionsMenu*					g_optionsMenu;
	//
	int GlobalInit()
	{
		g_timeCalculator			= new utility::TimeCalculator();
		g_textDisplay				= new TextDisplay();
		g_menu						= new Menu();
		g_textureDraw				= new TextureDraw();
		g_mainMenu					= new MainMenu( 500, 500 );
		g_gameHistory				= new GameHistory();
		g_optionsMenu				= new OptionsMenu( 100, 200 );
		//
		return RET_SUCCESS;
	}
	//
	int GlobalFree()
	{
		delete g_timeCalculator;
		delete g_textDisplay;
		delete g_menu;
		delete g_mainMenu;
		delete g_textureDraw;
		delete g_gameHistory;
		delete g_optionsMenu;
		//
		return RET_SUCCESS;
	}
}