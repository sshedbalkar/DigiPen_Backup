#include "Precompiled.h"

#include "menuSystem.h"
#include "XMLSerialization.h"
#include "Message.h"
#include "WindowsSystem.h"
#include "Core.h"
#include "GSM.h"
//#include "text.h"
#include "Audio.h"
namespace Framework
{

#define STATE_MAINMENU			0
#define STATE_PAUSEMENU			1
#define	STATE_GAMEPLAY			2
#define ISTATE_ON_ENTER			0
#define ISTATE_ON_UPDATE		1
#define ISTATE_ON_EXIT			2

MenuSystem* MENUSYSTEM = NULL;

//============================================================
MenuSystem::MenuSystem()
{
	MENUSYSTEM = this;
}
//============================================================
MenuSystem::~MenuSystem()
{
	Free();
}
//============================================================
void MenuSystem::Initialize()
{
	state = STATE_MAINMENU;
	istate = ISTATE_ON_ENTER;
	idle = false;
	onbutton=false;
}
//============================================================
void MenuSystem::Update( float timeslice )
{
}
//============================================================
void MenuSystem::Free()
{
	while( !menu_stack.empty() )
	{
		PopMenu();
	}
}
//============================================================
void MenuSystem::DisplayMenu( const char* filename )
{
}
//============================================================
void MenuSystem::PopMenu()
{
}
//============================================================
MenuSystem::MenuType* MenuSystem::GetMenu()
{
	return menu_stack.top();
}
//============================================================
void MenuSystem::SendMessage(Message* msg)
{

}
/***********************
	button functions
************************/
//============================================================
void MenuSystem::OnResume()
{
	CORE->pauseSystems(false);
	istate = ISTATE_ON_EXIT;
}
//============================================================
void MenuSystem::OnQuit()
{
	Message msg( Mid::Quit );
	CORE->BroadcastMessage( &msg );
}
//============================================================
void MenuSystem::OnRestart()
{
	CORE->gsm()->current()->restart();
	OnResume();
}

/***********
	GUI
***********/
//============================================================
void GUIComponent::SerializeVital( ISerializer& stream )
{
	stream.readValue( "x", x );
	stream.readValue( "y", y );
	stream.readValue( "width", width );
	stream.readValue( "height", height );
	stream.readValue( "title", title );
	
	int screen_width, screen_height;
//	GRAPHICS->GetScreenSize( &screen_width, &screen_height );
	//x *= (float)screen_width;
	//y *= (float)screen_height;
	//width *= (float)screen_width;
	//height *= (float)screen_height;
}
/*************
	button
*************/
//============================================================
GUIButton::GUIButton() : focused(false), GUIComponent(BUTTON),button_event(NULL)
{
}
//============================================================
void GUIButton::Serialize( ISerializer& stream )
{
	std::string event_name;
	SerializeVital( stream );
	stream.readValue("event", event_name );
	ButtonEventRouter( event_name );
}
//============================================================
void GUIButton::TriggerEvent()
{
	if ( button_event )
		(MENUSYSTEM->*button_event)();
}
//============================================================
void GUIButton::ButtonEventRouter( std::string& event_name )
{
	if ( event_name == "resume" )
		button_event = &MenuSystem::OnResume;
	if ( event_name == "quit" )
		button_event = &MenuSystem::OnQuit;
	if ( event_name == "restart" )
		button_event = &MenuSystem::OnRestart;
}

}