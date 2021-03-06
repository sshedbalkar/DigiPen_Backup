#include "Precompiled.h"
#include "Archetype.h"
#include "Utility.h"
#include <iostream>
#include "Composition.h"
//
namespace Framework
{
	Archetype::Archetype()
		:_stream(NULL)
	{
		//
	}
	Archetype::Archetype(ISerializer& stream)
		:_stream(stream.clone())
	{
		//
	}
	Archetype::~Archetype()
	{
		//delete _stream;
		safeDelete(_stream);
	}
	bool Archetype::createFrom(ISerializer& stream)
	{
		_stream = stream.clone();
		return true;
	}
	ISerializer* Archetype::getStream()
	{
		return _stream;
	}
	ISerializer* Archetype::getStream() const
	{
		return _stream;
	}
	bool Archetype::createFrom( GameObjectComposition *goc )
	{
		goc->Deserialize( _stream );
		return true;
	}
	////////////////////////////////
	ArchetypeCollection::ArchetypeCollection()
		:_archetypeMap()
	{

	}
	ArchetypeCollection::~ArchetypeCollection()
	{
		empty();
	}
	void ArchetypeCollection::createCollectionFrom(ISerializer& stream)
	{
		//empty();
		ISerializer* str = stream.clone();
		if(str->readProperty("Archetypes", *str))
		{
			std::vector<ISerializer*> archs;
			str->readProperty("Archetype", archs);
			//
			std::string id;
			for(std::vector<ISerializer*>::iterator it1 = archs.begin(), it2 = archs.end(); it1 != it2; ++it1)
			{
				if((*it1)->readProperty("id", id))
				{
					std::map<std::string, const Archetype*>::iterator ait = _archetypeMap.find( id );
					if ( ait == _archetypeMap.end() )
						_archetypeMap[id] = new Archetype(**it1);
					else
					{
						delete ait->second;
						ait->second = new Archetype( **it1 );
					}
				}
			}
			deletePointersVec(archs);
		}
		//
		delete str;
	}
	const Archetype* ArchetypeCollection::getArchetypeById(const std::string id) const
	{
		std::map<std::string, const Archetype*>::const_iterator it1 = _archetypeMap.find(id), it2 = _archetypeMap.end();
		if(it1 != it2)
		{
			return it1->second;
		}
		return NULL;
	}
	/*
	void ArchetypeCollection::testFn()
	{
		std::cout<<"Priniting Arch Collection\n";
		ISerializer* stream;
		std::string str;
		for(std::map<std::string, const Archetype*>::const_iterator it1 = _archetypeMap.begin(), it2 = _archetypeMap.end(); it1 != it2; ++it1)
		{
			stream = it1->second->getStream();
			stream->readProperty("id", str);
			std::cout<<"Arch Id: "<<it1->first<<", Value: "<<str<<"\n";
		}
	}
	*/
	void ArchetypeCollection::empty()
	{
		for(std::map<std::string, const Archetype*>::const_iterator it1 = _archetypeMap.begin(), it2 = _archetypeMap.end(); it1 != it2; ++it1)
		{
			delete it1->second;
		}
		_archetypeMap.clear();
	}
}