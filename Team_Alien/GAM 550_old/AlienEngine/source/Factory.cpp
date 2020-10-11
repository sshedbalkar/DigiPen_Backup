///////////////////////////////////////////////////////////////////////////////////////
//
//	Factory.cpp
//	
//	Authors: 
//	Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"

#include "Factory.h"
#include "Composition.h"
#include "ComponentCreator.h"
#include "GameData.h"
#include "Serialization.h"
#include "Utility.h"
#include "Core.h"
//
namespace Framework
{
	GameObjectFactory * FACTORY = NULL;

	GameObjectFactory::GameObjectFactory():
		_paused(false)
	{		
		ErrorIf( FACTORY!=NULL , "Factory already created");
		FACTORY = this;
		LastGameObjectId = 0;
	}

	GameObjectFactory::~GameObjectFactory()
	{
		//Delete all component creators
		ComponentMapType::iterator it = ComponentMap.begin();
		for(;it!=ComponentMap.end();++it)
			delete it->second;
	}

	GOC * GameObjectFactory::CreateEmptyComposition()
	{
		//Create the composition and give it an id
		//this function is used to create compositions
		//programmatically
		GOC * gameObject = new GOC();
		IdGameObject(gameObject);
		return gameObject;	
	}
	GOC* GameObjectFactory::BuildFromArchetype(const char* archId)
	{
		const Archetype *arch = GAMEDATA->archetypes()->getArchetypeById(archId);
		ISerializer* stream = arch->getStream()->clone();
		//stream->readProperty("Composition", *stream);
		GOC* obj = BuildAndSerialize(*stream);
		
		delete stream;
		return obj;
	}
	GOC* GameObjectFactory::BuildAndSerialize(const ISerializer& stream)
	{
		GOC* ret = NULL;
		std::string nm;
		//stream.readProperty("id", nm);
		ISerializer* strm = stream.clone();
		//
		strm->readProperty("Components", *strm);
		if(strm->isGood())
		{
			std::vector<ISerializer*> comps;
			if(strm->readProperty("Component", comps))
			{
				GOC* obj = CreateEmptyComposition();
				std::string type;
				for(std::vector<ISerializer*>::iterator it1 = comps.begin(), it2 = comps.end(); it1 != it2; ++it1)
				{
					(*it1)->readProperty("Type", type);
					ComponentMapType::iterator it = ComponentMap.find(type);
					ErrorIf(it == ComponentMap.end(), "Could not find component creator with id %s", type.c_str());
					if(it != ComponentMap.end())
					{
						//ComponentCreator is an object that creates the component
						ComponentCreator* creator = it->second;

						//Create the component by using the interface
						GameComponent* component = creator->Create();

						//Serialize the component with current stream (this will move
						//the stream to the next component
						//Note: this invokes the base case of serialization in which
						//the object serializes itself. component->Serialize( stream );
						streamRead(**it1, *component);
						//Add the new component to the composition
						if(creator->TypeId==CT_CollisionSphere || creator->TypeId==CT_CollisionBox)
							creator->TypeId=CT_CollisionPrimitive;
						obj->AddComponent(creator->TypeId, component);
					}
				}
				ret = obj;
			}
			deletePointersVec(comps);
		}
		delete strm;
		return ret;
	}


	std::vector<GOC *> GameObjectFactory::getVectorOfGOCOfSpecificType(int type){
	
		std::vector<GOC *> temp;
		GameObjectIdMapType::iterator p;
  
		 for(p = GameObjectIdMap.begin(); p != GameObjectIdMap.end(); p++) {
			if (p->second->CompositionTypeId==type){
				temp.push_back(p->second);
			}
		}
		 return temp;
	}
	//get GOC of specific type from map
	GOC * GameObjectFactory::getGOCofSpecificType(int type){
		GOC * temp=NULL;
		GameObjectIdMapType::iterator p;
  
		 for(p = GameObjectIdMap.begin(); p != GameObjectIdMap.end(); p++) {
			if (p->second->CompositionTypeId==type){
				temp=p->second;
				return temp;
			}
		}
		 return temp;
	}

	/*
	GOC * GameObjectFactory::BuildAndSerialize(const std::string& filename)
	{
		//Open the text file stream serializer
		TextSerializer stream;
		bool fileOpened = stream.Open(filename);
		ErrorIf( !fileOpened , "Could not open file %s. File does not exist or is protected." , filename.c_str() );
		std::string componentName;

		//Make sure the stream is valid
		if( stream.IsGood() )
		{
			//Create a new game object to hold the components
			GOC * gameObject = new GOC();

			while(stream.IsGood())
			{
				//Read the component's name
				StreamRead(stream,componentName);

				//Find the component's creator
				ComponentMapType::iterator it =  ComponentMap.find( componentName );
				ErrorIf( it == ComponentMap.end() , "Could not find component creator with name %s" , componentName.c_str() );
				if( it != ComponentMap.end() )
				{
					//ComponentCreator is an object that creates the component
					ComponentCreator * creator = it->second;

					//Create the component by using the interface
					GameComponent * component = creator->Create();

					//Serialize the component with current stream (this will move
					//the stream to the next component

					//Note: this invokes the base case of serialization in which
					//the object serializes itself. component->Serialize( stream );
					StreamRead(stream,*component);

					//Add the new component to the composition
					gameObject->AddComponent( creator->TypeId , component );
				}
			}

			//Id and initialize the game object composition
			IdGameObject(gameObject);

			return gameObject;
		}

		return NULL;
	}
	*/
	void GameObjectFactory::IdGameObject(GOC* gameObject)
	{
		//Just increment the last id used. Does not handle 
		//overflow but it would take over 4 billion objects
		//to break
		++LastGameObjectId;
		gameObject->ObjectId = LastGameObjectId;

		//Store the game object in the global object id map
		GameObjectIdMap[LastGameObjectId] = gameObject;
	}

	GOC * GameObjectFactory::GetObjectWithId(GOCId id)
	{
		GameObjectIdMapType::iterator it = GameObjectIdMap.find(id);
		if( it!=GameObjectIdMap.end())
			return it->second;
		else
			return NULL;
	}

	/*
	GOC * GameObjectFactory::Create(const std::string& filename)
	{
		GOC * goc = BuildAndSerialize(filename);
		if(goc) goc->Initialize();
		return goc;
	}
	*/
	GOC* GameObjectFactory::Create(const ISerializer& stream)
	{
		GOC* goc;
		std::string arch;
		stream.readProperty("archetype", arch);
		if(!arch.empty())
		{
			goc = BuildFromArchetype(arch.c_str());
		}
		else
		{
			goc = BuildAndSerialize(stream);
		}
		if(goc) goc->Initialize();
		return goc;
	}
	void GameObjectFactory::SendMessage(Message *message)
	{
		if (message->MessageId == Mid::SystemPause)
		{
			const SystemPause *m = static_cast<const SystemPause*>(message);
			_paused = m->Pause;
		}
	}

	void GameObjectFactory::Destroy(GOC * gameObject)
	{
		//Add the object to the to be deleted list they will be delete
		//when the factory is updated
		ObjectsToBeDeleted.insert(gameObject);
	}

	void GameObjectFactory::Update(float dt)
	{
		static_cast<void>(dt);
		if(!_paused)
		{
			//Delete all objects in the ObjectsToBeDeleted list 

			std::set<GOC*>::iterator it = ObjectsToBeDeleted.begin();
			for(;it!=ObjectsToBeDeleted.end();++it)
			{
				GOC * gameObject = *it;
				GameObjectIdMapType::iterator idItr =  GameObjectIdMap.find( gameObject->ObjectId );
				ErrorIf( idItr == GameObjectIdMap.end(), "Object %d was double deleted or is bad memory.", gameObject->ObjectId);
				if( idItr!= GameObjectIdMap.end() )
				{
					//Delete it and remove its entry in the Id map
					delete gameObject;
					GameObjectIdMap.erase( idItr );
				}
			}
			//All objects to be delete have been deleted
			ObjectsToBeDeleted.clear();
		}
	}

	//Bind the creator for this component with the component name
	void GameObjectFactory::AddComponentCreator(const std::string& name,ComponentCreator* creator)
	{
		ComponentMap[name] = creator;
	}

	//Clean up the game world
	void GameObjectFactory::DestroyAllObjects()
	{
		GameObjectIdMapType::iterator it =  GameObjectIdMap.begin();
		for(;it!=GameObjectIdMap.end();++it)
			delete it->second;

		GameObjectIdMap.clear();
	}
	bool GameObjectFactory::Deserialize( ISerializer* stream )
	{
		for( GameObjectIdMapType::iterator it1 = GameObjectIdMap.begin(), it2 = GameObjectIdMap.end(); it1 != it2; ++it1 )
		{
			it1->second->Deserialize( stream );
		}
		return true;
	}
}