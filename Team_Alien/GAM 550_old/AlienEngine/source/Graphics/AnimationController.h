#pragma once

#include "Component.h"

namespace Framework
{

class Transform;
struct Animation;
	
class AnimationController : public GameComponent
{
public:
	AnimationController();
	~AnimationController();
	
	virtual void Serialize( ISerializer& stream );

	int* track_data;
	Animation* anim;
	float anim_time;
	bool looping;
};

}
