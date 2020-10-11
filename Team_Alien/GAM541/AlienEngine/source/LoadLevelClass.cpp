#include "Precompiled.h"
#include "GameLogic.h"
#include "Graphics.h"
#include "ComponentCreator.h"
#include "Camera.h"
#include "DebugTools.h"
#include "XMLSerialization.h"
#include "Utility.h"
#include "InteractiveObjProperties.h"
//#include "ComponentRegistration.h"
#include "LoadLevelClass.h"
#include "HeroProperties.h"
#include "AI_Type1.h"
#include "AI_Type2.h"
#include "debugGeometry.h"

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
		LOGIC->CAMERA = 0;
		LOGIC->HERO = 0;
		LOGIC->FINALBOSS = 0;
		LOGIC->ListOfButtons.clear();
		LOGIC->ListOfDoorsToBeActivated.clear();
		LOGIC->ListOfGOCsDying.clear();
		LOGIC->ListOfEnemies.clear();
		LOGIC->ListOfBoxes.clear();
		std::cout<<"Cleared Game logic data\n";
	}

	GOC* LoadLevelClass::CreateObjectAt(const char* archId, Vec2& position, float rotation, Vec3& scale, Vec2& bodyScale)
	{
		//std::cout<<"Creating obj with Arch: "<<archId<<", pos: "<<position<<", rotation: "<<rotation<<", scale: "<<scale<<"\n";
		GOC * newObject = FACTORY->BuildFromArchetype(archId);
		newObject->archId=archId;
		//Get the transform and adjust the position
		Transform * transform = newObject->has(Transform);
		
		transform->Position = Vec3(position.x, position.y, 0.0f);
		transform->RotationX = rotation;
		transform->Scale = scale;

		Body * body = newObject->has(Body);
		if(body)
		{
			body->Position=Vec2(position.x, position.y);
			if (body->BodyShape->Id==Shape::SidBox)
			{
				body->BodyScale=bodyScale;
				static_cast<ShapeAAB*>(body->BodyShape)->Extents=body->BodyScale;
				if(DrawDebug)
				{
					DebugGeometry *debug=new DebugGeometry();
					debug->AddDebugShape(DEBUG_SHAPE_RECTANGLE, Vec3(bodyScale.x*2,bodyScale.y*2,0), Vec4(1,0,0,1));
					newObject->AddComponent(CT_DebugGeometry,debug);
				}
			}
		}
		
		if (strcmp(archId, "arc_1")==0)	{
			newObject->CompositionTypeId=GOC_Type_Hero ;
			HeroProperties  *hp=new HeroProperties();
			newObject->AddComponent(CT_HeroProperties,hp);
			LOGIC->HERO=newObject;
		}
		else if (strcmp(archId, "button_platform1")==0){
			newObject->CompositionTypeId=GOC_Type_ButtonPlatform1  ;
			LOGIC->ListOfButtons.push_back(newObject);
		}
		else if (strcmp(archId, "button_platform2")==0){
			newObject->CompositionTypeId=GOC_Type_ButtonPlatform2   ;
			LOGIC->ListOfButtons.push_back(newObject);
		}
		else if (strcmp(archId, "mirror_0")==0 || strcmp(archId, "mirror_1")==0){
			newObject->CompositionTypeId=GOC_Type_Mirror  ;
		}
		else if (strcmp(archId, "enemy_1")==0){
			newObject->CompositionTypeId=GOC_Type_Enemy1  ;
			AI_Type1  *AI=new AI_Type1();
			newObject->AddComponent(CT_AI_Type1,AI);
			LOGIC->ListOfEnemies.push_back(newObject);
			
		}
		else if (strcmp(archId, "platform_test0")==0){
			newObject->CompositionTypeId=GOC_Type_Platform1  ;
		}
		else if (strcmp(archId, "door_0")==0){
			newObject->CompositionTypeId=GOC_Type_Door1  ;
			LOGIC->ListOfDoorsToBeActivated.push_back(newObject);
		}
		else if (strcmp(archId, "door_1")==0){
			newObject->CompositionTypeId=GOC_Type_Door2  ;
			LOGIC->ListOfDoorsToBeActivated.push_back(newObject);
		}
		else if (strcmp(archId, "box_0")==0){
			newObject->CompositionTypeId=GOC_Type_MovingBox1  ;
			LOGIC->ListOfBoxes.push_back(newObject);
			
		}
		else if (strcmp(archId, "pillar_0")==0){
			newObject->CompositionTypeId=GOC_Type_Pillar   ;			
			//newObject->has(Transform)->Position.z+=1;
		}
		else if (strcmp(archId, "boxOfDeath")==0){
			newObject->CompositionTypeId=GOC_Type_InvisibleBoxOfDeath   ;
		}
		else if (strcmp(archId, "deathplatform_0")==0){
			newObject->CompositionTypeId=GOC_Type_Platform_Death   ;
		}
		else if (strcmp(archId, "water_box")==0){
			newObject->CompositionTypeId=GOC_Type_WaterPlatform  ;
			newObject->has(Transform)->Position.z-=1;
		}
		else if (strcmp(archId, "boss_1")==0){
			newObject->CompositionTypeId=GOC_Type_Boss1  ;
			LOGIC->FINALBOSS=newObject;
			AI_Type2  *AI=new AI_Type2();
			newObject->AddComponent(CT_AI_Type2,AI);
			LOGIC->FINALBOSS=newObject;
		}
		else if (strcmp(archId, "bossInvinsibleObstacle")==0){
			newObject->CompositionTypeId=GOC_Type_InvisibleBox1  ;
		}
		/*else if (strcmp(archId, "platform_special")==0){
			newObject->CompositionTypeId=GOC_Type_SpecialMovingPlatform  ;
			LOGIC->BossSpecialPlatform=newObject;
		}*/


			
		//Initialize the composition
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
					
					//
					stream->goToParent();
					if(stream->readProperty("Position", *stream))
					{
						stream->readValue("value", pos);
						stream->goToParent();
						if(stream->readProperty("Rotation", *stream))
						{
							stream->readValue("value", rot);
							stream->goToParent();
							if(stream->readProperty("Scale", *stream))
							{
								stream->readValue("value", scl);
								stream->goToParent();
								if (stream->readProperty("BodyScale", *stream))
								{
									stream->readValue("value", bodyscl);
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

							
				if (obj->has(InteractiveObjProperties)){
					stream->goToParent();
					obj->has(InteractiveObjProperties)->Serialize(*stream);
				}
			
			}
			//
			deletePointersVec(comps);
			delete str;
		}
		delete reader;
		//
		LOGIC->CAMERA=FACTORY->CreateEmptyComposition();
		Transform * cameratransform = new Transform();
		cameratransform->Position= Vec3( -29.0f,0.0f,-45.0f );
		LOGIC->CAMERA->AddComponent( CT_Transform, cameratransform );
		LOGIC->CAMERA->AddComponent( CT_Camera , new Camera() );
		LOGIC->CAMERA->Initialize();
	}


}
