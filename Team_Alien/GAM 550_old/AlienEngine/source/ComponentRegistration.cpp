#include "Precompiled.h"

#include "ComponentRegistration.h"
#include "ComponentCreator.h"
//
#include "Transform.h"
#include "text.h"
#include "camera.h"
#include "factory.h"
//#include "InteractiveObjProperties.h"
//#include "StoreMessage.h"
//#include "HeroProperties.h"
//#include "AI_Type1.h"
//#include "AI_Type2.h"
//
namespace Framework
{
	ComponentRegistration::ComponentRegistration()
	{
		RegisterComponent(Transform);
		RegisterComponent(Text);
		RegisterComponent(Camera);
		//RegisterComponent(InteractiveObjProperties);
		//RegisterComponent(StoreMessage);
		//RegisterComponent(HeroProperties);
		//RegisterComponent(AI_Type1);
		//RegisterComponent(AI_Type2);
	}
	ComponentRegistration::~ComponentRegistration()
	{
		//
	}
};
