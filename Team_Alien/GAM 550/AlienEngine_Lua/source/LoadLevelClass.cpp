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
#include "FileRW.h"
#include "ActivationProperties.h"

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

	GOC* LoadLevelClass::CreateObjectAt(ObjectParameters parameters)
	{
		GOC * newObject = FACTORY->BuildFromArchetype(parameters.Arch_id.c_str() );
		newObject->archId=parameters.Arch_id;
		
		newObject->CompositionTypeId=newObject->GetIdFromName(const_cast<char*>(parameters.Arch_id.c_str()));
		
		Transform *t=newObject->has(Transform);
		t->Position=parameters.Position ;
		t->Scale=parameters.Scale ;

		Quaternion a=Quaternion(parameters.Quaternion.w,parameters.Quaternion.x,parameters.Quaternion.y,parameters.Quaternion.z);
		a.normalise();
		t->QUATERNION=D3DXQUATERNION(a.i,a.j,a.k,a.r);

		if(newObject->has(CollisionPrimitive))
		{
			if(newObject->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Box)
			{
				static_cast<CollisionBox*>(newObject->has(CollisionPrimitive))->halfSize.x=parameters.Scale.x/2.0;
				static_cast<CollisionBox*>(newObject->has(CollisionPrimitive))->halfSize.y=parameters.Scale.y/2.0;
				static_cast<CollisionBox*>(newObject->has(CollisionPrimitive))->halfSize.z=parameters.Scale.z/2.0;
			}else if(newObject->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Sphere)
			{
				static_cast<CollisionSphere*>(newObject->has(CollisionPrimitive))->radius=parameters.Scale.x/2.0;
			}
		}
		if(newObject->has(ActivationProperties))
		{
			newObject->has(ActivationProperties)->m_ActivationId=parameters.ObjectActivationId ;
			newObject->has(ActivationProperties)->m_ActivatedByVec = parameters.ActivatedBy ;
		}

		newObject->Initialize();
		return newObject;
	}

	
	void LoadLevelClass::loadGameSettings(const std::string& filename)
	{
		_gameData = new GameData();
		_gameData->initialize();
		GAMEDATA->loadLevelData(filename.c_str());
		//
		utility::FileInfoList flist;
		utility::ListFilesInDirectory( "Objects/arch*.xml", flist );
		//
		for each( utility::FileInfo f in flist )
		{
			GAMEDATA->LoadArchetypeData( f.FullName.c_str() );
		}
	}
	void LoadLevelClass::LoadLevelFile(const std::string& filename)
	{
		std::cout<<"Opening Level file: "<<filename.c_str()<<"\n";
		XMLReader *reader = new XMLReader();
		if(reader->open(filename.c_str()))
		{
			ISerializer* str = reader->data().clone();
			std::vector<ISerializer*> comps;
			if ( str->readProperty( "items", *str ) )
			{
				str->readProperty("item", comps);
				//
				for(std::vector<ISerializer*>::iterator it1 = comps.begin(), it2 = comps.end(); it1 != it2; ++it1)
				{
					ISerializer* stream = *it1;
					ObjectParameters objPar;
					if ( stream->readProperty("archetype", objPar.Arch_id ) )
					{
						GOC* obj;
						float arr[4];
						int x;
						
						if(stream->readValue("Position", arr, 3))
						{
							objPar.Position.x = arr[0];
							objPar.Position.y = arr[1];
							objPar.Position.z = arr[2];
						}if(stream->readValue("Scale", arr, 3))
						{
							objPar.Scale.x = arr[0];
							objPar.Scale.y = arr[1];
							objPar.Scale.z = arr[2];
						}
						if(stream->readValue("Orientation", arr, 4))
						{
							objPar.Quaternion.w = arr[3];
							objPar.Quaternion.x = arr[0];
							objPar.Quaternion.y = arr[1];
							objPar.Quaternion.z = arr[2];
						}

						if(stream->readValue("ObjectActivationId", x))
						{
							objPar.ObjectActivationId =x;

						}

						if(stream->readValue("ActivatedBy",  arr, 4))
						{
							objPar.ActivatedBy.w =arr[0];
							objPar.ActivatedBy.x =arr[1];
							objPar.ActivatedBy.y =arr[2];
							objPar.ActivatedBy.z =arr[3];

						}

						obj=CreateObjectAt(objPar);
						AddGOCtoLogicContainer(obj);
					}	
						
				}
			}
			//
			utility::deletePointersVec(comps);
			delete str;
		}
		delete reader;
		//
		//SaveLevelLocal( "Testfile.xml" );
	}
	

	void LoadLevelClass::AddGOCtoLogicContainer(GOC* newGOC)
	{
		if(newGOC->has(ActivationProperties))
			LOGIC->m_ActivatableObjectsList.push_back(newGOC);
	}

}
