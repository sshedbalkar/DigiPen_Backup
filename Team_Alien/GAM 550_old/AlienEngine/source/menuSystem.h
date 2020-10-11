#pragma once //Makes sure this header is only included once

#include "System.h"
#include "Message.h"
#include <stack>

namespace Framework
{

class GUIComponent;
class Text;
class ISerializer;

enum MenuID
{
	MainMenu,
	PauseMenu
};

class MenuSystem : public ISystem
{
public:
	typedef std::deque<GUIComponent*> MenuType;
private:
	typedef std::stack<MenuType*> MenuStackType;

private:
	MenuStackType menu_stack;
	int state;
	int istate;
	float idle;
	bool onbutton;
public:
	MenuSystem();
	virtual ~MenuSystem();

	virtual void SendMessage(Message* message);
	virtual void Update(float timeslice);
	virtual std::string GetName() { return "MenuSystem";};
	virtual void Initialize();
	virtual void Load(){}
	virtual void Unload(){}
	virtual void Free();

	void DisplayMenu( const char* filename );
	void PopMenu();
	MenuType* GetMenu();
	void SetDisableSystem() { idle = true;};

	//button functions
public:
	void OnResume();
	void OnQuit();
	void OnRestart();
};
extern MenuSystem* MENUSYSTEM;


enum GUIType
{
	TEXT,
	BUTTON
};
//===========================
class GUIComponent
{
public:
	float x;
	float y;
	float width;
	float height;
	std::string title;
	GUIType type;

public:
	GUIComponent( GUIType _type ) :type(_type) {};

	virtual void Serialize( ISerializer& stream ) = 0;
protected:
	void SerializeVital( ISerializer& stream );
};
//===========================
class GUIText : public GUIComponent
{
public:
	GUIText() : GUIComponent(TEXT) {};
	void Serialize( ISerializer& stream ) { SerializeVital(stream); };
};
//===========================
class GUIButton : public GUIComponent
{
public:
	bool focused;
	void (MenuSystem::*button_event)();
public:
	GUIButton();
	void Serialize( ISerializer& stream );
	void ButtonEventRouter( std::string& event_name );
	void TriggerEvent();
};

}
