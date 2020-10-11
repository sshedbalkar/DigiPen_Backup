#include "Precompiled.h"
#include "XMLSerialization.h"
#include "Utility.h"
#include "SaveLevelClass.h"
#include "XMLSerialization.h"
#include <fstream>
#include "ActivationProperties.h"

namespace Framework
{

	SaveLevelClass* SaveClass=NULL;

	#define DrawDebug false

	void SaveLevelClass::Initialize()
	{
	}

	SaveLevelClass::SaveLevelClass()
	{
		SaveClass=this;
	}

	SaveLevelClass::~SaveLevelClass()
	{
		delete _gameData;
	}

	void SaveLevelClass::SaveLevel(const std::string& fileName)
	{
		const int len=222;
		char str[len];
		//const char* fileName = "test.xml";

		std::cout<<"Saving Level file..\n";

		std::fstream os;
		os.open ( fileName.c_str(), std::ios::binary | std::fstream::in | std::fstream::out | std::fstream::trunc );
		////
		if( os.is_open() )
		{
			XMLSerializer root;
			root.allocateNew();
			ISerializer* stream = root.clone();
			stream->writeNode( "items" );
			//
			GameObjectIdMapType mapOfGOCS = FACTORY->GetAllObjects() ;
			ISerializer *str1, *str2;
			
			GameObjectIdMapType::iterator p;
			for(p = mapOfGOCS.begin(); p != mapOfGOCS.end(); p++)	
			{
				if (p->second->archId =="")
					continue;//dont save	
				str1 = stream->clone();
				str1->writeNode( "item" );
				str1->readProperty( "item", *str1 );
				str1->writeProperty( "archetype", p->second->archId.c_str() ) ;
				
				if (p->second->has(Transform))
				{
					sprintf_s( str, len, "%4.2f,%4.2f,%4.2f", p->second->has(Transform)->Position.x,  p->second->has(Transform)->Position.y, p->second->has(Transform)->Position.z);
					str1->writeNodeAndValue( "Position", str );
				}

				if (p->second->has(Transform))
				{
					sprintf_s( str, len, "%4.2f,%4.2f,%4.2f", p->second->has(Transform)->Scale.x,  p->second->has(Transform)->Scale.y, p->second->has(Transform)->Scale.z);
					str1->writeNodeAndValue( "Scale", str );
				}
				
				
				if (p->second->has(Transform))
				{
					sprintf_s( str, len, "%4.4f,%4.4f,%4.4f,%4.4f",  p->second->has(Transform)->QUATERNION.x , p->second->has(Transform)->QUATERNION.y ,p->second->has(Transform)->QUATERNION.z ,p->second->has(Transform)->QUATERNION.w);
					str1->writeNodeAndValue( "Orientation", str );
				}

				if (p->second->has(ActivationProperties))
				{
					sprintf_s( str, len, "%d",  p->second->has(ActivationProperties)->GetActivationId());
					str1->writeNodeAndValue( "ObjectActivationId", str );
					sprintf_s( str, len, "%4.0f,%4.0f,%4.0f,%4.0f",  p->second->has(ActivationProperties)->GetObjectActivatedByVec().w ,   p->second->has(ActivationProperties)->GetObjectActivatedByVec().x, p->second->has(ActivationProperties)->GetObjectActivatedByVec().y, p->second->has(ActivationProperties)->GetObjectActivatedByVec().z);
					str1->writeNodeAndValue( "ActivatedBy", str );
				}



				str2 = str1->clone();
				str1->goToParent();
				str1->readProperty( "items", *str1 );
				stream->removeNode( "item" );
				str1->appendNode( *str2 );
			}			
			XMLSerializer::saveNodeToStream( root, &os );
			os.close();
			std::cout<<"Level file save as: "<<fileName<<"\n";
		}
		else
		{
			std::cout<<"Can't save file: " <<fileName<<"\n";
		}
	}
}
