#include "Precompiled.h"
#include "ComponentCreator.h"
#include "XMLSerialization.h"
#include "Utility.h"
//#include "ComponentRegistration.h"
#include "LoadLevelClass.h"
#include "scriptEngine.h"
#include "XMLSerialization.h"
#include <fstream>

namespace Framework
{


	void LoadLevelClass::Initialize()
	{
		//ComponentRegistration();
		loadGameSettings();
		//LoadLevelFile("Objects\\Level_0.xml");
	}

	LoadLevelClass::LoadLevelClass()
	{
	}

	LoadLevelClass::~LoadLevelClass()
	{
		delete _gameData;
	}
	void LoadLevelClass::freeLogicData()
	{
	}

	GOC* LoadLevelClass::CreateObjectAt(const char* archId, float* position )
	{
		//std::cout<<"Creating obj with Arch: "<<archId<<", pos: "<<position<<", rotation: "<<rotation<<", scale: "<<scale<<"\n";
		GOC * newObject = FACTORY->BuildFromArchetype(archId);
		newObject->archId=archId;
		//Get the transform and adjust the position
		Transform * transform = newObject->has(Transform);
		
		if ( transform ) transform->Position = Vec3(position[0], position[1], position[2] );
		
		//Initialize the composition
		newObject->Initialize();
		
		return newObject;
	}

	void LoadLevelClass::loadGameSettings( )
	{
		_gameData = new GameData();
		_gameData->initialize();
		std::string path = "objects\\";

		WIN32_FIND_DATA file;
		HANDLE search_handle = FindFirstFile( "objects\\arch*.xml", &file );
		if ( search_handle )
		{
			do
			{
				std::string filename = path + file.cFileName;
				GAMEDATA->LoadArchetypeData(filename.c_str());
			}
			while ( FindNextFile( search_handle, &file ) );
		}
//		CloseHandle( search_handle );
	}
	bool LoadLevelClass::LoadLevelFile(const std::string& filename)
	{
		bool ret = true;
		XMLReader *reader = new XMLReader();
		if(reader->open(filename.c_str()))
		{
			ISerializer* str = reader->data().clone();
			std::vector<ISerializer*> comps;
			//
			if ( str->readProperty( "items", *str ) )
			{
				str->readProperty("item", comps);
				//
				for(std::vector<ISerializer*>::iterator it1 = comps.begin(), it2 = comps.end(); it1 != it2; ++it1)
				{
					ISerializer* stream = *it1;
					std::string arch;
					if ( stream->readProperty("archetype", arch) )
					{
						GOC* obj;
						float pos[3] = {0.0f};
						//
						stream->readValue("position", pos, 3);
						obj = CreateObjectAt( arch.c_str(), pos );
						//
						obj->CompositionTypeId = GOC_Type_TestGOC;
						//std::cout<<"Arch type: "<<obj->GetName()<<"\n";
					}
				}
				deletePointersVec(comps);
				str->goToParent();
			}
			//
			delete str;
		}
		//
		else ret = false;
		delete reader;
		//
		//SaveLevelLocal( "Testfile.xml" );
		return ret;
	}
	bool LoadLevelClass::SaveLevelLocal( const char *fileName )
	{
		std::fstream os;
		os.open ( fileName, std::ios::binary | std::fstream::in | std::fstream::out | std::fstream::trunc );
		//
		if( os.is_open() )
		{
			XMLSerializer str1;
			str1.allocateNew();
			//
			FACTORY->Deserialize( &str1 );
			XMLSerializer::saveNodeToStream( str1, &os );
			//
			os.close();
			return true;
		}
		else
		{
			std::cout<<"Can't open file: " <<fileName<<"\n";
			return false;
		}
	}

}
