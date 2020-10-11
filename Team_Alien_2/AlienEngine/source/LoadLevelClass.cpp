#include "Precompiled.h"
#include "GameLogic.h"
#include "Graphics.h"
#include "Camera.h"
#include "DebugTools.h"
#include "XMLSerialization.h"
#include "Utility.h"
#include "LoadLevelClass.h"
#include "debugGeometry.h"
#include "XMLSerialization.h"
#include <fstream>

namespace Framework
{
	LoadLevelClass* LevelClass=NULL;

	#define DrawDebug false

	void LoadLevelClass::Initialize()
	{
		//ComponentRegistration();
		loadGameSettings("Objects\\gameSettings.xml");
		//LoadLevelFile("Objects\\Level_0.xml");
	}

	LoadLevelClass::LoadLevelClass()
	{
		LevelClass=this;
	}

	LoadLevelClass::~LoadLevelClass()
	{
		delete _gameData;
	}
	void LoadLevelClass::freeLogicData()
	{
		std::cout<<"Cleared Game logic data\n";
	}

	GOC* LoadLevelClass::CreateObjectAt(const char* archId, Vec2& position, float rotation, Vec3& scale, Vec2& bodyScale)
	{
		//std::cout<<"Creating obj with Arch: "<<archId<<", pos: "<<position<<", rotation: "<<rotation<<", scale: "<<scale<<"\n";
		GOC * newObject = FACTORY->BuildFromArchetype(archId);
		newObject->archId=archId;
		////Get the transform and adjust the position
		//Transform * transform = newObject->has(Transform);
		//
		//transform->Position = Vec3(position.x, position.y, 0.0f);
		//transform->RotationX = rotation;
		//transform->Scale = scale;

		//Body * body = newObject->has(Body);
		//if(body)
		//{
		//	body->Position=Vec2(position.x, position.y);
		//	if (body->BodyShape->Id==Shape::SidBox)
		//	{
		//		body->BodyScale=bodyScale;
		//		static_cast<ShapeAAB*>(body->BodyShape)->Extents=body->BodyScale;
		//		if(DrawDebug)
		//		{
		//			DebugGeometry *debug=new DebugGeometry();
		//			debug->AddDebugShape(DEBUG_SHAPE_RECTANGLE, Vec3(bodyScale.x*2,bodyScale.y*2,0), Vec4(1,0,0,1));
		//			newObject->AddComponent(CT_DebugGeometry,debug);
		//		}
		//	}
		//}
		//
		//if (strcmp(archId, "arc_1")==0)	{
		//	
		//}
		//else if (strcmp(archId, "button_platform1")==0){
		//	
		//}
		//else if (strcmp(archId, "button_platform2")==0){
		//	
		//}
		
		newObject->Initialize();

		return newObject;
	}
	GOC* LoadLevelClass::CreateObjectAt(const char* archId, Vec2& position, float rotation,Vec3& scale)
	{
		return CreateObjectAt(archId, position, rotation, scale, Vec2(scale.x/2.0f,scale.y/2.0f));
	}

	GOC* LoadLevelClass::CreateObjectAt(const char* archId, Vec2& position, float rotation)
	{
		return CreateObjectAt(archId, position, rotation, Vec3(1.0f, 1.0f, 1.0f));
	}
	GOC* LoadLevelClass::CreateObjectAt(const char* archId, Vec2& position)
	{
		return CreateObjectAt(archId, position, 0.0f, Vec3(1.0f, 1.0f, 1.0f));
	}
	void LoadLevelClass::loadGameSettings(const std::string& filename)
	{
		_gameData = new GameData();
		_gameData->initialize();
		GAMEDATA->loadLevelData(filename.c_str());
	}
	void LoadLevelClass::LoadLevelFile(const std::string& filename)
	{
		std::cout<<"Opening Level file: "<<filename.c_str()<<"\n";
		XMLReader *reader = new XMLReader();
		if(reader->open(filename.c_str()))
		{
			ISerializer* str = reader->data().clone();
			str->readProperty("Level", *str);
			str->readProperty("Layers", *str);
			str->readProperty("Layer", *str);
			str->readProperty("Items", *str);
			//
			std::vector<ISerializer*> comps;
			str->readProperty("Item", comps);
			//
			for(std::vector<ISerializer*>::iterator it1 = comps.begin(), it2 = comps.end(); it1 != it2; ++it1)
			{
				ISerializer* stream = *it1;
				std::string id, arch;
				//
				stream->readProperty("ArchetypeId", *stream);
				
				stream->readValue("value", arch);
				//std::cout<<arch<<std::endl;
				GOC* obj;

		
				//
				if(!arch.empty())
				{
					Vec2 pos,bodyscl;
					float rot;
					Vec3 scl;
					float arr[3];
					//
					stream->goToParent();
					if(stream->readProperty("Position", *stream))
					{
						//stream->readValue("value", pos);
						stream->readValue("value", arr, 2);
						pos.x = arr[0];
						pos.y = arr[1];
						stream->goToParent();
						if(stream->readProperty("Rotation", *stream))
						{
							stream->readValue("value", rot);
							stream->goToParent();
							if(stream->readProperty("Scale", *stream))
							{
								//stream->readValue("value", scl);
								stream->readValue("value", arr, 3);
								scl.x = arr[0];
								scl.y = arr[1];
								scl.z = arr[2];
								//
								stream->goToParent();
								if (stream->readProperty("BodyScale", *stream))
								{
									//stream->readValue("value", bodyscl);
									stream->readValue("value", arr, 2);
									bodyscl.x = arr[0];
									bodyscl.y = arr[1];
									obj=CreateObjectAt(arch.c_str(), pos, rot, scl,bodyscl);
								}
								else
								{
									
									obj=CreateObjectAt(arch.c_str(), pos, rot, scl);
								}
								
							}
							else
							{
								obj=CreateObjectAt(arch.c_str(), pos, rot);
							}
						}
						else
						{
							obj=CreateObjectAt(arch.c_str(), pos);
						}
					}
					else
					{
						obj = FACTORY->Create(*stream);
						
					}
					
				}
				else
				{
					obj = FACTORY->Create(*stream);
				}

							
						
			}
			//
			deletePointersVec(comps);
			delete str;
		}
		delete reader;
		//
		//SaveLevelLocal( "Testfile.xml" );
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
