#pragma once
//
namespace utility
{
	class TimeCalculator;
}
//
namespace Memory
{
	class MemAllocatorGen;
	//
	void PrintLeaks();
	void PrintStats();
	extern bool GAME_EXIT;
}
//
namespace Framework
{
	class TextDisplay;
	class Menu;
	class TextureDraw;
	class MainMenu;
	class GameHistory;
	class OptionsMenu;
	//
	extern utility::TimeCalculator*		g_timeCalculator;
	extern TextDisplay*					g_textDisplay;
	extern Menu*						g_menu;
	extern TextureDraw*					g_textureDraw;
	extern MainMenu*					g_mainMenu;
	extern OptionsMenu*					g_optionsMenu;
	extern GameHistory*					g_gameHistory;
	extern Memory::MemAllocatorGen*		g_memAllocator;
	//
	int									GlobalInit();
	int									GlobalFree();
	
}