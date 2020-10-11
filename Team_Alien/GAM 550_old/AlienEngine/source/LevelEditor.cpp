#include "Precompiled.h"
#include "lua.h"
#include "LevelEditor.h"
#include "Utility.h"
#include "GameData.h"
#include "Archetype.h"
#include "Factory.h"

#include "luaLibrary.h"

namespace Framework
{
LevelEditor* LuaLibraryEditor::leveleditor;
//====================================================================================
int LuaLibraryEditor::Open( lua_State* L, void* editor )
{
	leveleditor = static_cast<LevelEditor*>( editor );
	
	luaL_reg editor_lib[] =
	{
		{ "HideBars", EditorHideBars },
		{ "ShowBars", EditorShowBars },
		{ "GetObject", GetObject },
		{ NULL, NULL }
	};
	
	luaL_openlib( L, "editor", editor_lib, 0 );
	return 0;
}
//====================================================================================
int LuaLibraryEditor::EditorHideBars( lua_State* L )
{
	leveleditor->HideBars();
	return 0;
}
//====================================================================================
int LuaLibraryEditor::EditorShowBars( lua_State* L )
{
	leveleditor->ShowBars();
	return 0;
}

int LuaLibraryEditor::GetObject( lua_State* L )
{
	char defines[80];
	GOC** goc = static_cast<GOC**>( luaL_checkudata( L, 1, "LuaBook.goc" ) );
	leveleditor->selectedGOC = *goc;
	if (leveleditor->selectedGOC != NULL)
	{
		sprintf_s( defines, " %s visible = true ", "ObjectDetails" );
		TwDefine( defines );
		leveleditor->ShowObjectDetails();
	}
	else
	{
		sprintf_s( defines, " %s visible = false ", "ObjectDetails" );
		TwDefine( defines );
	}

	return 0;
}

//====================================================================================
LevelEditor::LevelEditor()
{
}
//====================================================================================
LevelEditor::~LevelEditor()
{
	for ( unsigned int i = 0; i < twbarlist.size(); ++i )
	{
		delete twbarlist[i];
		twbarlist[i] = NULL;
	}
	twbarlist.clear();

	TwTerminate();
}
//====================================================================================
void LevelEditor::Initialize()
{
	LuaLibrary::Add( LuaLibraryEditor::Open, this );
	
	
}

void LevelEditor::ShowObjectDetails ()
{
	TwBar* bar = GetTWBar( "ObjectDetails" );
	TwRemoveAllVars(bar);

	TwAddVarRW( bar, "X", TW_TYPE_FLOAT, &(selectedGOC->has(Transform)->Position.x), " step=0.1 group=Position label='Y Position' " );	
	TwAddVarRW( bar, "Y", TW_TYPE_FLOAT, &(selectedGOC->has(Transform)->Position.y), " step=0.1 group=Position label='Y Position' " );	
	TwAddVarRW( bar, "Z", TW_TYPE_FLOAT, &(selectedGOC->has(Transform)->Position.z), " step=0.1 group=Position label='Z Position' " );	
	//sprintf_s( defines, " min = 0 max = %d step = 1 ", 1 );
	//TwAddVarRW( bar, "archetype no.", TW_TYPE_INT32, &selected, defines );
	
	//TwAddButton( bar, "create", ArchetypeSelectionBar::BuildFromArchetype, this, NULL );
}
void LevelEditor::ShowTerrainDetails ()
{
	printf( "vvv\n");
	TwBar* bar = GetTWBar( "Terrain" );

	//TwRemoveAllVars(bar);

	/*TwAddVarRW( bar, "X", TW_TYPE_FLOAT, &(selectedGOC->has(Transform)->Position.x), " step=0.1 group=Position label='Y Position' " );	
	TwAddVarRW( bar, "Y", TW_TYPE_FLOAT, &(selectedGOC->has(Transform)->Position.y), " step=0.1 group=Position label='Y Position' " );	
	TwAddVarRW( bar, "Z", TW_TYPE_FLOAT, &(selectedGOC->has(Transform)->Position.z), " step=0.1 group=Position label='Z Position' " );	*/
}

//====================================================================================
void LevelEditor::InitializeTweakBar( ID3D11Device* device, int width, int height )
{
	
	TwInit(TW_DIRECT3D11, device); // for Direct3D 11

	TwDefine( " GLOBAL fontsize=3 " );
//	TwWindowSize( width, height );
	TwDefine( " TW_HELP visible = false " );

	twbarlist.push_back( new ArchetypeSelectionBar() );
	InitializeBars();
	HideBars();
}

void LevelEditor::InitializeBars()
{
	//OBJECT
	TweakBarControl* bar;
	bar  =  new TweakBarControl("ObjectDetails",300,300,300,300,false);
	barlist.insert( std::pair<const char*, TweakBarControl*>( bar->GetName(),bar ) );

	//TERRAIN
	bar =  new TweakBarControl("Terrain",400,50,300,300,false);
	
	//terrainSel = PUSH;
 //	TwEnumVal TerrainSelections[] = { {PUSH, "PUSH"}, {PULL, "PULL"}, {TEXTURE, "TEXTURE"}, {NORMAL_MAPPING, "NORMAL_MAPPING"} };
	//TwType TerrainSelectionsType;
 //	// Defining season enum type
	//TerrainSelectionsType = TwDefineEnum("TerrainSelectionsType", TerrainSelections, 4);
	//// Adding season to bar
	//TwAddVarRW(bar, "Select", TerrainSelectionsType, &terrainSel, NULL);

	//TwAddButton(bar, "SelectTerrain", LevelEditor::SelectTerrain, this, NULL);

	//TwAddSeparator(bar, "sep1", NULL);
	//TwDefine(" Terrain/sep1 ");

	//sprintf_s( defines, " %s label = '%s' refresh = 0.5 position = '%d %d' size='300 300' alpha=100.0 valueswidth=120 visible = false", "Terrain", "Terrain",400,50 );
	//TwDefine( defines );
	barlist.insert( std::pair<const char*, TweakBarControl*>( bar->GetName(),bar ) );
}

void LevelEditor::SelectTerrain(void * client_data)
{
	LevelEditor* editor = static_cast<LevelEditor*>(client_data);
	editor->ShowTerrainDetails();
}


TwBar* LevelEditor::GetTWBar( const char* s )
{
	std::map<const char*,TweakBarControl*>::iterator it;
	it=barlist.find(s);
	barlist.end();
	return it->second->GetTWBar();

}



//====================================================================================
void LevelEditor::Update(float dt)
{
	for ( unsigned int i = 0; i < twbarlist.size(); ++i )
	{
		twbarlist[i]->Update();
	}

}
//====================================================================================
void LevelEditor::SendMessage(Message * m)
{

}
//====================================================================================
void LevelEditor::HideBars()
{
	char defines[80];
	for ( unsigned int i = 0; i < twbarlist.size(); ++i )
	{
		sprintf_s( defines, " %s visible = false ", twbarlist[i]->GetTitle() );
		TwDefine( defines );
	}
	std::map<const char*,TweakBarControl*>::iterator it;
	for(it = barlist.begin(); it != barlist.end(); ++it){
		it->second->HideBar();
	}
}
//====================================================================================
void LevelEditor::ShowBars()
{
	char defines[80];
	for ( unsigned int i = 0; i < twbarlist.size(); ++i )
	{
		sprintf_s( defines, " %s visible = true ", twbarlist[i]->GetTitle() );
		TwDefine( defines );
	}
	std::map<const char*,TweakBarControl*>::iterator it;
	for(it = barlist.begin(); it != barlist.end(); ++it){
		it->second->ShowBar();
	}

}
//==================================================================================== TweakBarControl


TweakBarControl::TweakBarControl(const char* s,int posX,int posY,int sizeX, int sizeY, bool visible):m_name(s),m_posX(posX),m_posY(posY),m_sizeX(sizeX),m_sizeY(sizeY),m_visible(visible)
{
	m_TwBar=TwNewBar( m_name  );
	sprintf_s( defines, " %s label = '%s' refresh = 0.5 position = '%d %d' size='%d %d' alpha=100.0 valueswidth=120 visible = %s", m_name,  m_name, m_posX, m_posY, m_sizeX, m_sizeY, (m_visible)?"true":"false" );
	TwDefine( defines );
	

}

TweakBarControl::~TweakBarControl()
{

}

void TweakBarControl::Initialize()
{

}

const char* TweakBarControl::GetName()
{
	return m_name;
}

TwBar* TweakBarControl::GetTWBar() //temp remove later
{
	return m_TwBar;
}

void TweakBarControl::HideBar()
{
	sprintf_s( defines, " %s visible = false ", m_name  );
	TwDefine( defines );
}
void TweakBarControl::ShowBar()
{
	sprintf_s( defines, " %s visible = true ", m_name );
	TwDefine( defines );
}

const char* TweakBarControl::GetStringFromEnum(MyEnum e)
{
  switch (e)
  {
	  case Object: return "ObjectDetails";
	  case Terrain: return "Terrain";
	 // default: throw Exception("Bad MyEnum");
  }
}

TweakBarControl::MyEnum TweakBarControl::GetEnumFromString(const char* s)
{
	if (std::strcmp(s,"ObjectDetails")==0)
		return TweakBarControl::MyEnum::Object ;
	else if (std::strcmp(s,"Terrain")==0)
		return TweakBarControl::MyEnum::Terrain ;
}




//==================================================================================== Archetype selection bar
ArchetypeSelectionBar::ArchetypeSelectionBar() : title( "archetypes" ), selected( 0 )
{
	char defines[255];
	TwBar* bar = TwNewBar( title.c_str() );

	ArchetypeCollection* archetypes = NULL;
	if ( GAMEDATA ) archetypes = GAMEDATA->archetypes();

	if ( !archetypes ) return;
	
	sprintf_s( defines, " %s label = '%s' refresh = 0.5 position = '16 16' size='300 100' alpha=100.0 valueswidth=120 ", title.c_str(), title.c_str() );
	TwDefine( defines );

	std::map<std::string, const Archetype*>& archetypemap = archetypes->GetArchetypeMap();
	archetype_ids = new const char* [ archetypemap.size() ];
	std::map<std::string, const Archetype*>::iterator it;
	int i = 0;
	for ( it = archetypemap.begin(); it != archetypemap.end(); ++it )
	{
		archetype_ids[i] = it->first.c_str();
		++i;
	}
	
	selected_id = *archetype_ids;
	selected = 0;

	TwAddVarRO( bar, "archetype id", TW_TYPE_CDSTRING, &selected_id, NULL );	
	sprintf_s( defines, " min = 0 max = %d step = 1 ", i-1 );
	TwAddVarRW( bar, "archetype no.", TW_TYPE_INT32, &selected, defines );


	TwAddButton( bar, "create", ArchetypeSelectionBar::BuildFromArchetype, this, NULL );
}
//====================================================================================
ArchetypeSelectionBar::~ArchetypeSelectionBar()
{
	delete [] archetype_ids;
	archetype_ids = NULL;
}
//====================================================================================
void ArchetypeSelectionBar::Update()
{
	selected_id = *( archetype_ids + selected );
}
//====================================================================================
void ArchetypeSelectionBar::BuildFromArchetype( void* client_data )
{
	ArchetypeSelectionBar* bar = static_cast<ArchetypeSelectionBar*>( client_data );
	GOC* object = FACTORY->BuildFromArchetype( bar->selected_id );
	object->Initialize();
}


}
