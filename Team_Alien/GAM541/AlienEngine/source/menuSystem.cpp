#include "Precompiled.h"
#include "menuSystem.h"
#include "XMLSerialization.h"
#include "Message.h"
#include "WindowsSystem.h"
#include "Core.h"
#include "GSM.h"
#include "text.h"
#include "Graphics.h"
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
	if ( idle ) return;
	switch( state )
	{
	case STATE_MAINMENU:
		switch( istate )
		{
		case ISTATE_ON_ENTER:
			{
				MessageMenu m;
				m.menu_id = MainMenu;
				CORE->BroadcastMessage( &m );
				istate = ISTATE_ON_UPDATE;
				DisplayMenu( "Objects\\pause_menu.xml");
			}
			break;
		case ISTATE_ON_UPDATE:
			break;
		case ISTATE_ON_EXIT:
			{
				MessageLeaveMenu m;
				m.menu_id = MainMenu;
				CORE->BroadcastMessage( &m );
				PopMenu();
				state = STATE_GAMEPLAY;
				istate = ISTATE_ON_ENTER;
			}
			break;
		}
		break;
		
	case STATE_PAUSEMENU:
		switch( istate )
		{
		case ISTATE_ON_ENTER:
			{
				MessageMenu m;
				m.menu_id = PauseMenu;
				CORE->BroadcastMessage( &m );
				istate = ISTATE_ON_UPDATE;
				DisplayMenu( "Objects\\pause_menu.xml");
			}
			break;
		case ISTATE_ON_UPDATE:
			break;
		case ISTATE_ON_EXIT:
			{
				MessageLeaveMenu m;
				m.menu_id = PauseMenu;
				CORE->BroadcastMessage( &m );
				PopMenu();
				state = STATE_GAMEPLAY;
				istate = ISTATE_ON_ENTER;
			}
			break;
		}
		break;
		
	case STATE_GAMEPLAY:
		break;
	}
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
	XMLReader reader;
	std::string type;
	std::vector<ISerializer*> components;
	GUIComponent* component;
	ISerializer* stream;
	MenuType* menu;
	reader.open( filename );
	//ErrorIf( !, "file %s not found.\n", filename );
	stream = reader.data().clone();
	stream->readProperty( "component", components );
	menu = new MenuType;

	for ( unsigned int i = 0; i < components.size(); ++i )
	{
		components[i]->readProperty("type", type );
		if ( type == "button" ) component = new GUIButton;
		else if ( type == "text" ) component = new GUIText;

		component->Serialize( *components[i] );
		menu->push_back( component );
		delete components[i];
	}
	menu_stack.push( menu );
	delete stream;

}
//============================================================
void MenuSystem::PopMenu()
{
	if ( menu_stack.empty() ) return;
	MenuType* menu = menu_stack.top();
	MenuType::iterator it;
	for ( it = menu->begin(); it != menu->end(); ++it )
	{
		delete (*it);
	}
	delete menu;
	menu_stack.pop();
}
//============================================================
MenuSystem::MenuType* MenuSystem::GetMenu()
{
	return menu_stack.top();
}
//============================================================
void MenuSystem::SendMessage(Message* msg)
{
	if ( idle ) return;
	if ( msg->MessageId == Mid::CharacterKey )
	{	
		MessageCharacterKey* m = (MessageCharacterKey*)msg;

		if ( m->character == VK_ESCAPE )
		{
			CORE->togglePauseSystems();
			//
			if ( menu_stack.empty() && state == STATE_GAMEPLAY) 
			{
				state = STATE_PAUSEMENU;
				istate = ISTATE_ON_ENTER;
			}
			else if ( (!menu_stack.empty()) && state == STATE_PAUSEMENU )
			{
				OnResume();
			}
		}
		else if ( m->character == 'q' )
		{
			//CORE->gsm()->current()->restart(); this should be in the menu
		}
	/*	else if ( m->character == VK_RETURN )
		{
			if ( (!menu_stack.empty()) && state == STATE_MAINMENU  )
			{
				OnResume();
			}
		}*/
	}
	else if ( msg->MessageId == Mid::LMouseButton )
	{

		if ( !menu_stack.empty() )
		{
			LMouseButton* m = (LMouseButton*)msg;
			MenuType* menu = menu_stack.top();
			MenuType::iterator it;
			for ( it = menu->begin(); it != menu->end(); ++it )
			{
				if ( (*it)->type != BUTTON ) continue;
				const Vec2* cpos = GRAPHICS->GetCursorPos();
				float x = cpos->x;
				float y = cpos->y;

				x -= (*it)->x; x *= 2.0f;
				y -= (*it)->y; y *= 2.0f;
				if ( abs(x) < (*it)->width && abs(y) < (*it)->height )
				{
					GUIButton* button = (GUIButton*)(*it);
					button->TriggerEvent();
					break;
				}
				
			}//end for
		}//end if
	}else if(msg->MessageId == Mid::MouseMove)
	{
		if ( !menu_stack.empty() )
		{
			bool temp=true;
			MouseMove* m = (MouseMove*)msg;
			MenuType* menu = menu_stack.top();
			MenuType::iterator it;
			for ( it = menu->begin(); it != menu->end(); ++it )
			{
				if ( (*it)->type != BUTTON ) continue;
				const Vec2* cpos = GRAPHICS->GetCursorPos();
				float x = cpos->x;
				float y = cpos->y;
				x -= (*it)->x; x *= 2.0f;
				y -= (*it)->y; y *= 2.0f;
				if ( abs(x) < (*it)->width && abs(y) < (*it)->height )
				{
					temp=false;
					if(onbutton)
					{
						AUDIO->Stop("button");
						AUDIO->Play("button");
						onbutton=false;
					}
				}				
			}//end for
			if(temp)
				onbutton=true;
		}//end if
	}
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
	GRAPHICS->GetScreenSize( &screen_width, &screen_height );
	x *= (float)screen_width;
	y *= (float)screen_height;
	width *= (float)screen_width;
	height *= (float)screen_height;
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