///////////////////////////////////////////////////////////////////////////////////////
//
//	WinMain
//	The main entry point for the game--everything starts here.
//	
//	Authors:  , 
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "WindowsSystem.h"
#include "Global.h"
#include "TestClass1.h"
using namespace Framework;

//The title the window will have at the top


//void EnableMemoryLeakChecking(int breakAlloc=-1)
//{
//	//Set the leak checking flag
//	int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
//	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
//	_CrtSetDbgFlag(tmpDbgFlag);
//
//	//If a valid break alloc provided set the breakAlloc
//	 if(breakAlloc!=-1) _CrtSetBreakAlloc( breakAlloc );
//}

//The entry point for the application--called automatically when the game starts.
//The first parameter is a handle to the application instance.
//The second parameter is the previous app instance which you can use to prevent being launched multiple times
//The third parameter is the command line string, but use GetCommandLine() instead.
//The last parameter is the manner in which the application's window is to be displayed (not needed).

INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
//	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//srand(unsigned(time(NULL)));
//#ifdef _DEBUG
//	Debug::DebugTools::getInstance()->enable(true);
//	EnableMemoryLeakChecking();
//#else
////#define std::cout(...) ((void)0)
//#endif
	//WNDCLASSEX wc;

	//#ifdef _DEBUG
	//EnableMemoryLeakChecking();
	//#endif
	
	std::cout<<"Alien Main...\n";
	Memory::TestClass1* t1 = new Memory::TestClass1();
	/*
	//Create the core engine which manages all the systems that make up the game
	CoreEngine* engine = new CoreEngine();

	//Initialize the game by creating the various systems needed and adding them to the core engine
	WindowsSystem* windows = new WindowsSystem(windowTitle, ClientWidth, ClientHeight);

	wc.hIcon= LoadIcon(windows->hInstance , "Assets/textures/Icon.ico");
	//
	GlobalInit();
	Graphics * graphics = new Graphics();
	graphics->SetWindowProperties(windows->hWnd, ClientWidth, ClientHeight);

	engine->AddSystem(windows);
	engine->AddSystem(new GameObjectFactory());
	engine->AddSystem(new Physics());
	engine->AddSystem(new GameLogic());
	engine->AddSystem(graphics);
	engine->AddSystem(new Audio());
	engine->AddSystem(new LevelEditor());
	//TODO: Add additional systems, such as audio, and possibly xinput, lua, etc.
	engine->Initialize();
	
	//Everything is set up, so activate the window
	windows->ActivateWindow();

	//inform the application that the game is about to begin
	engine->FirstFrameRun();

	//Run the game
	engine->GameLoop();

	//Delete all the game objects
	FACTORY->DestroyAllObjects();

	//Delete all the systems
	engine->DestroySystems();

	//Delete the engine itself
	delete engine;
	//
	GlobalFree();
	//
	//#ifdef _DEBUG
	//::_CrtDumpMemoryLeaks();
	//#endif
	*/
	//Memory::PrintLeaks();
	//Memory::PrintStats();
	//Game over, application will now close
	//std::cin.get();
	return 0;
}




