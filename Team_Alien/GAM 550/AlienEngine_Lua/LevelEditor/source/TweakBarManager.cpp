#include "TweakBarManager.h"
#include "Physics.h"
#include "Graphics.h"
#include "GameData.h"
#include "ActivationProperties.h"


namespace Framework
{

TweakBarManager::TweakBarManager()
{
	m_selectedGOC=NULL; m_archEnumId=0;
	//myStructType = TwDefineStruct("MyStructType", myStructMembers, 3, sizeof(MyStruct), NULL, NULL);
}


void TweakBarManager::SetSelectedGOC(GOC * g)
{
	m_selectedGOC=g;
	ShowObjectDetails();
}

std::string tbString="";
void TweakBarManager::ShowObjectDetails ()
{
	if(m_selectedGOC)
	{
		TweakBarControl* c_bar = GetTweakBarControl( GetStringFromEnum(MyEnum::Object) );
		c_bar->ShowBar();
		
		TwRemoveAllVars(c_bar->GetTWBar());
		tbString=m_selectedGOC->archId;
		TwAddVarRO(c_bar->GetTWBar(), "ArchId", TW_TYPE_STDSTRING, &tbString, " label='Archetype: ' ");
		TwAddSeparator(c_bar->GetTWBar(), "sep1", NULL);
		//pos
		TwAddVarRW( c_bar->GetTWBar(), "XPos", TW_TYPE_FLOAT, &(m_selectedGOC->has(Transform)->Position.x), " step=0.1 group=Position label='X Position' " );	
		TwAddVarRW( c_bar->GetTWBar(), "YPos", TW_TYPE_FLOAT, &(m_selectedGOC->has(Transform)->Position.y), " step=0.1 group=Position label='Y Position' " );	
		TwAddVarRW( c_bar->GetTWBar(), "ZPos", TW_TYPE_FLOAT, &(m_selectedGOC->has(Transform)->Position.z), " step=0.1 group=Position label='Z Position' " );	
		//scale
		if (m_selectedGOC->has(CollisionPrimitive) && m_selectedGOC->has(CollisionPrimitive)->Id == CollisionPrimitive::ShapeId::Sphere)
		{
			TwAddVarCB(c_bar->GetTWBar(), "Radius", TW_TYPE_FLOAT, TweakBarManager::SetRadius, TweakBarManager::GetRadius, this, "min= 0.5 step=0.1 group=Scale label='Radius' ");
		}
		else
		{
			TwAddVarRW( c_bar->GetTWBar(), "XScale", TW_TYPE_FLOAT, &(m_selectedGOC->has(Transform)->Scale.x), "min= 0.5 step=0.1 group=Scale label='X Scale' " );	
			TwAddVarRW( c_bar->GetTWBar(), "YScale", TW_TYPE_FLOAT, &(m_selectedGOC->has(Transform)->Scale.y), "min= 0.5 step=0.1 group=Scale label='Y Scale' " );	
			TwAddVarRW( c_bar->GetTWBar(), "ZScale", TW_TYPE_FLOAT, &(m_selectedGOC->has(Transform)->Scale.z), "min= 0.5 step=0.1 group=Scale label='Z Scale' " );	
		}//orientation
		TwAddVarRW( c_bar->GetTWBar(), "Orientation", TW_TYPE_QUAT4F, &(m_selectedGOC->has(Transform)->QUATERNION), " opened=true axisz=-z group=Rotation label='Rotation' " );	
		TwAddButton( c_bar->GetTWBar(), "Rotate90X", TweakBarManager::Rotate90X, this, " group=Rotation label='Rotate 90 on X' ");
		TwAddButton( c_bar->GetTWBar(), "Rotate90Y", TweakBarManager::Rotate90Y, this, " group=Rotation label='Rotate 90 on y' ");
		TwAddButton( c_bar->GetTWBar(), "Rotate90Z", TweakBarManager::Rotate90Z, this, " group=Rotation label='Rotate 90 on Z' ");

		if (m_selectedGOC->has(ActivationProperties))
		{
			TwAddSeparator(c_bar->GetTWBar(), "sep2", NULL);
			TwAddVarRW( c_bar->GetTWBar(), "ObjectActivationId", TW_TYPE_INT32, &(m_selectedGOC->has(ActivationProperties)->m_ActivationId), " step=1 min=1 max=100 group=ActivationProperties label='ObjectActivationId' " );	
			TwAddVarCB(c_bar->GetTWBar(), "ActivatedBy", ActivationPropertiesStructType, TweakBarManager::SetActivatedBy, TweakBarManager::GetActivatedBy, this, "group=ActivationProperties label='Activated By' ");
		}
	}
	else
	{
		TweakBarControl* c_bar = GetTweakBarControl( GetStringFromEnum(MyEnum::Object) );
		c_bar->HideBar();
	}
}

void TW_CALL TweakBarManager::SetRadius (const void *value, void *clientData)
{
	static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(Transform)->Scale.x  = (*static_cast<const float *>(value)) ;
	static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(Transform)->Scale.y = (*static_cast<const float *>(value)) ;
	static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(Transform)->Scale.z = (*static_cast<const float *>(value)) ;
}
void TW_CALL TweakBarManager::GetRadius(void *value, void *clientData)
{
	(*static_cast<float *>(value)) = static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(Transform)->Scale.x;
}

void TW_CALL TweakBarManager::SetActivatedBy (const void *value, void *clientData)
{
	static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.w = (*static_cast<const ActivationPropertiesStruct *>(value)).Object1  ;
	static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.x = (*static_cast<const ActivationPropertiesStruct *>(value)).Object2 ;
	static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.y = (*static_cast<const ActivationPropertiesStruct *>(value)).Object3 ;
	static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.z = (*static_cast<const ActivationPropertiesStruct *>(value)).Object4 ;
}
void TW_CALL TweakBarManager::GetActivatedBy (void *value, void *clientData)
{
	(*static_cast<ActivationPropertiesStruct *>(value)).Object1 = static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.w;
	(*static_cast<ActivationPropertiesStruct *>(value)).Object2 = static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.x;
	(*static_cast<ActivationPropertiesStruct *>(value)).Object3 = static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.y;
	(*static_cast<ActivationPropertiesStruct *>(value)).Object4 = static_cast<TweakBarManager*>(clientData)->m_selectedGOC->has(ActivationProperties)->m_ActivatedByVec.z;
}


void TweakBarManager::ShowCreateObjectDetails()
{
	TweakBarControl* c_bar = GetTweakBarControl( GetStringFromEnum(MyEnum::CreateObject ) );
	c_bar->ShowBar();

	if(m_archetypes.size()==0)
	{
		std::map<std::string, const Archetype*>  tempmap = GAMEDATA->archetypes()->getArchetypeMap();
	
		int i=0;
		std::string str("");
	
		std::map<std::string,const Archetype*>::iterator it;
		for(it = tempmap.begin(); it != tempmap.end(); ++it){
			m_archetypes.insert( std::pair<int, std::string>(i, it->first) );
			str.append(it->first);
			str.append(",");
			++i;
		}
		str = str.substr(0, str.size()-1);
		TwType archetypeType;
		archetypeType = TwDefineEnumFromString("Archetype", str.c_str());
		TwAddVarRW(c_bar->GetTWBar(), "Archetype", archetypeType, &m_archEnumId , NULL);
		TwAddButton( c_bar->GetTWBar(), "Create", TweakBarManager::BuildFromArchetype, this, NULL );
	}
}
	
void TW_CALL TweakBarManager::BuildFromArchetype(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	if(mnagerP->m_archetypes.size()>0)
	{
		GOC *g;
		std::map<int,std::string>::iterator it;
		it=mnagerP->m_archetypes.find(mnagerP->m_archEnumId);
		if ( it != mnagerP->m_archetypes.end( ) ){
			g=FACTORY->BuildFromArchetype(it->second.c_str());
			Vector3 temp=EDITOR->CenterPos + EDITOR->CenterDir*10;
			g->has(Transform)->Position=Vec3(temp.x,temp.y,temp.z);
			//g->has(Transform)->Scale=Vec3(1,1,1);
			D3DXQUATERNION q(0,0,0,1);
			g->has(Transform)->QUATERNION=q;
			g->Initialize();
		}
	}
}

void TW_CALL TweakBarManager::Rotate90X(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	D3DXQUATERNION q;
	q=calculateQuaterion(R_PI/2.0,Vec3(1,0,0));
	mnagerP->m_selectedGOC->has(Transform)->QUATERNION*=q;

}
void TW_CALL TweakBarManager::Rotate90Y(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	D3DXQUATERNION q;
	q=calculateQuaterion(R_PI/2.0,Vec3(0,1,0));
	mnagerP->m_selectedGOC->has(Transform)->QUATERNION*=q;

}
void TW_CALL TweakBarManager::Rotate90Z(void *clientData)
{
	TweakBarManager * mnagerP=static_cast<TweakBarManager *>(clientData);
	D3DXQUATERNION q;
	q=calculateQuaterion(R_PI/2.0,Vec3(0,0,1));
	mnagerP->m_selectedGOC->has(Transform)->QUATERNION*=q;

}

void TW_CALL TweakBarManager::CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString)
{
  destinationClientString = sourceLibraryString;
}
 


void TweakBarManager::InitializeTweakBar( )
{	
	GRAPHICS->initializeAntTweakBar();
	TwCopyStdStringToClientFunc(CopyStdStringToClient); // must be called once (just after TwInit for instance)
		
	TwDefine( " GLOBAL fontsize=2 " );
	TwStructMember myStructMembers[] = {
	 { "Object1", TW_TYPE_FLOAT, offsetof(ActivationPropertiesStruct, Object1), "label='Object 1'" },
	 { "Object2", TW_TYPE_FLOAT, offsetof(ActivationPropertiesStruct, Object2), "label='Object 2'" },
	 { "Object3", TW_TYPE_FLOAT, offsetof(ActivationPropertiesStruct, Object3), "label='Object 3'" },
	 { "Object4", TW_TYPE_FLOAT, offsetof(ActivationPropertiesStruct, Object4), "label='Object 4'" }

	 };
	ActivationPropertiesStructType = TwDefineStruct("ActivationPropertiesStructType", myStructMembers, 4, sizeof(ActivationPropertiesStruct), NULL, NULL);


	InitializeBars();
	//HideAllBars();
}

void TweakBarManager::InitializeBars()
{
	//OBJECT
	TweakBarControl* bar;
	bar  =  new TweakBarControl(GetStringFromEnum(MyEnum::Object),50,50,300,500,false);
	m_twbarlist.insert( std::pair<const char*, TweakBarControl*>( bar->GetName(),bar ) );


	//ARCHETYPE
	bar  =  new TweakBarControl(GetStringFromEnum(MyEnum::CreateObject),GRAPHICS->screen_width-300,50,250,300,false);
	m_twbarlist.insert( std::pair<const char*, TweakBarControl*>( bar->GetName(),bar ) );
	
}

TweakBarControl* TweakBarManager::GetTweakBarControl( const char* s )
{
	std::map<const char*,TweakBarControl*>::iterator it;
	it=m_twbarlist.find(s);
	m_twbarlist.end();
	return it->second;

}

void TweakBarManager::HideAllBars()
{
	std::map<const char*,TweakBarControl*>::iterator it;
	for(it = m_twbarlist.begin(); it != m_twbarlist.end(); ++it){
		it->second->HideBar();
	}
}
void TweakBarManager::ShowAllBars()
{
	std::map<const char*,TweakBarControl*>::iterator it;
	for(it = m_twbarlist.begin(); it != m_twbarlist.end(); ++it){
		it->second->ShowBar();
	}

}

void TweakBarManager::Update()
{
	if(m_selectedGOC)
	{
		if(m_selectedGOC->has(CollisionPrimitive))
		{
			if(m_selectedGOC->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Box)
			{
				static_cast<CollisionBox*>(m_selectedGOC->has(CollisionPrimitive))->halfSize.x=m_selectedGOC->has(Transform)->Scale.x/2.0;
				static_cast<CollisionBox*>(m_selectedGOC->has(CollisionPrimitive))->halfSize.y=m_selectedGOC->has(Transform)->Scale.y/2.0;
				static_cast<CollisionBox*>(m_selectedGOC->has(CollisionPrimitive))->halfSize.z=m_selectedGOC->has(Transform)->Scale .z/2.0;
			}else if(m_selectedGOC->has(CollisionPrimitive)->Id==CollisionPrimitive::ShapeId::Sphere)
			{
				static_cast<CollisionSphere*>(m_selectedGOC->has(CollisionPrimitive))->radius=m_selectedGOC->has(Transform)->Scale .x/2.0;
			}
		}
	}
}

const char* TweakBarManager::GetStringFromEnum(MyEnum e)
{
  switch (e)
  {
	  case Object: return "ObjectDetails";
	  case CreateObject: return "CreateObject";
	 // default: throw Exception("Bad MyEnum");
  }
}

TweakBarManager::MyEnum TweakBarManager::GetEnumFromString(const char* s)
{
	if (std::strcmp(s,"ObjectDetails")==0)
		return TweakBarManager::MyEnum::Object ;
	else if (std::strcmp(s,"Terrain")==0)
		return TweakBarManager::MyEnum::CreateObject ;
}

//==================================================================================== TweakBarManager





}
