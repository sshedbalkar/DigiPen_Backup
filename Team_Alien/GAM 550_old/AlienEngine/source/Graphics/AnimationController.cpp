#include "Precompiled.h"
#include "AnimationController.h"
#include "XMLSerialization.h"
#include "animation.h"

namespace Framework
{
	
//=========================================================
AnimationController::AnimationController() : anim( NULL )
{
	track_data = new int[ MAX_BONES];
	ZeroMemory( track_data, sizeof(int) * MAX_BONES );
}
//=========================================================
AnimationController::~AnimationController()
{
	if ( track_data ) delete [] track_data; track_data = NULL;
}
//=========================================================
void AnimationController::Serialize( ISerializer& stream )
{
	anim_time = 0.0f;
	looping = true;
}

}
