#pragma once //Makes sure this header is only included once
#include "Engine.h"
#include "VMath.h"


struct lua_State;

namespace Framework
{

class ArchetypeCollection;
class LevelEditor;
class TweakBarControl;

//////////////////////////////////////////////////////
class LuaLibraryEditor
{
	static LevelEditor* leveleditor;
public:
	static int Open( lua_State* L, void* editor );
	static int EditorHideBars( lua_State* L );
	static int EditorShowBars( lua_State* L );
	static int GetObject( lua_State* L );


};
	
///////////////////////////////////////////////////
class TweakBar
{
public:
	virtual const char* GetTitle() = 0;
	virtual void Update() {};
};

////////////////////////////////////////////////////
class LevelEditor : public ISystem
{
	friend class LuaLibraryEditor;

	typedef std::vector< TweakBar* > TweakBarListType;

private:
	ArchetypeCollection* archetypes;
	TweakBarListType twbarlist;
	GOC* selectedGOC;
	char defines[255];
	typedef std::map<const char*, TweakBarControl*> TweakBarList;
	TweakBarList barlist;
	//typedef enum { PUSH, PULL, TEXTURE, NORMAL_MAPPING } TerrainSelections;
	//TerrainSelections terrainSel;

public:
	LevelEditor();
	~LevelEditor();
	virtual std::string GetName(){return "LevelEditor";}
	virtual void SendMessage(Message *);
	virtual void Update(float t);
	virtual void Initialize();
	void ShowObjectDetails();
	void ShowTerrainDetails();
	TwBar* GetTWBar(const char*);

	static void __stdcall SelectTerrain(void* client_data); 

	void InitializeTweakBar( ID3D11Device* device, int width, int height );
	void InitializeBars();

private:
	void HideBars();
	void ShowBars();

};


////////////////////////////////////////////////////
class TweakBarControl
{
	//typedef std::vector< TweakBar* > TweakBarListType;

private:
	char defines[255];
	//typedef std::map<std::string, TwBar* > TweakBarList;
	//TweakBarList barlist;
	typedef enum { PUSH, PULL, TEXTURE, NORMAL_MAPPING } TerrainSelections;
	TerrainSelections m_terrainSel;
	
	const char* m_name;
	int m_posX, m_posY;
	int m_sizeX, m_sizeY;
	bool m_visible;
	TwBar * m_TwBar;
	enum MyEnum {Object,Terrain};//add new types here

public:
	TweakBarControl(const char* s,int posX,int posY,int sizeX, int sizeY, bool visible);
	~TweakBarControl();
	void Initialize();
	TwBar* GetTWBar(); //temp remove later
	const char * GetName();
	const char* GetStringFromEnum(MyEnum);
	MyEnum GetEnumFromString(const char* s);
	//static void __stdcall SelectTerrain(void* client_data); 

	void HideBar();
	void ShowBar();

	

};


////////////////////////////////////////////////////


class ArchetypeSelectionBar : public TweakBar
{
	std::string title;
	int selected;
	const char** archetype_ids;
	const char* selected_id;

public:
	ArchetypeSelectionBar();
	~ArchetypeSelectionBar();
	virtual const char* GetTitle() { return title.c_str(); }
	virtual void Update();
	
	static void __stdcall GetSelectArchetype( void* value, void* client_data );
	static void __stdcall BuildFromArchetype( void* client_data );
};


}