#include "Precompiled.h"
#include "GraphicsObject.h"
#include "graphics.h"
#include "XMLSerialization.h"
#include "material.h"
#include "Composition.h"
#include "Transform.h"
#include "text.h"
#include "camera.h"
#include "AnimationController.h"

namespace Framework
{

//===========================================
GraphicsComposition::GraphicsComposition() : transform( NULL ), text( NULL ), animctrl( NULL )
{
}
//===========================================
GraphicsComposition::~GraphicsComposition()
{
	GraphicsObjectListType::iterator it;
	it = instance_list.begin();
	while ( it != instance_list.end() )
	{
		delete *it;
		*it = NULL;
		++it;
	}
	instance_list.clear();
}
//===========================================
void GraphicsComposition::Serialize( ISerializer& stream )
{
	std::vector<ISerializer*> str_insts;

	if ( !stream.readValue( "flag", flag ) ) flag = 0;

	stream.readProperty( "material", str_insts );

	for ( unsigned int i = 0; i < str_insts.size(); ++i )
	{
		GraphicsObject* inst = new GraphicsObject(this);
		inst->Serialize( *str_insts[i] );
		instance_list.push_back( inst );
		delete str_insts[i]; str_insts[i] = NULL;
	}

}
//===========================================
void GraphicsComposition::Initialize()
{
	transform = GetOwner()->has(Transform);
	text = GetOwner()->has(Text);
	camera = GetOwner()->has(Camera);
	animctrl = GetOwner()->has(AnimationController);
}
//===========================================
void GraphicsComposition::AddGraphicsObject( GraphicsObject* go )
{
	instance_list.push_back( go );
}
//===========================================
GraphicsObject::GraphicsObject( GraphicsComposition* _owner) : data(NULL), owner(_owner), material(NULL)
{
}
//===========================================
GraphicsObject::~GraphicsObject()
{
	if ( material ) material->RemoveObject( this );
}
//===========================================
void GraphicsObject::Serialize( ISerializer& stream )
{
	std::string mat_id;
	stream.readProperty( "id", mat_id );
	material = GRAPHICS->GetMaterial( mat_id.c_str() );
	if ( material == NULL ) printf("null\n");
	else material->AddAndSerializeObject( this, &stream );
}
//===========================================
void GraphicsObject::Initialize( Material* _material, ISerializer* stream )
{
	material = _material;
	if ( material ) material->AddAndSerializeObject( this, stream );
}
//===========================================
int GraphicsObject::Flag()
{
	return owner->flag;
}
//===========================================
Transform* GraphicsObject::Transform()
{
	return owner->transform;
}
//===========================================
Text* GraphicsObject::Text()
{
	return owner->text;
}
//===========================================
Camera* GraphicsObject::Camera()
{
	return owner->camera;
}
//===========================================
AnimationController* GraphicsObject::AnimCtrl()
{
	return owner->animctrl;
}

}
