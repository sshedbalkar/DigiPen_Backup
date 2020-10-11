#include <D3DX10math.h>
#include <fstream>
#include "Containers.h"
#include "animation.h"
#include "AnimationController.h"

namespace Framework
{

Skeleton::Skeleton() : bones( NULL )
{
}
Skeleton::~Skeleton()
{
	if ( bones ) delete [] bones;
	bones = NULL;
}
Track::Track() : kframes(NULL)
{
}
Track::~Track()
{
	if ( kframes ) delete [] kframes;
	kframes = NULL;
}
//==========================================================================
Animation::Animation() : skeleton( NULL )
{
}
//==========================================================================
void Animation::Load( std::ifstream& fin )
{
	/////////////////////////////////// temporary variables //////////////////////////////////////
	int num_bones;
	char anim_name[32];
	/////////////////////////////////// load skeleton ////////////////////////////////////////////
	skeleton = new Skeleton;
	fin.read( (char*)&num_bones, sizeof(int) );
	skeleton->num_bones = num_bones;
	skeleton->bones = new Bone[num_bones];
	//fin.read( (char*)skeleton->bones, sizeof(Bone) * num_bones );
	for ( int i = 0; i < num_bones; ++i )
	{
		fin.read( (char*)&skeleton->bones[i].parent, sizeof(int) );
		D3DXQUATERNION quat;
		float trans[3];
		fin.read( (char*)trans, sizeof(float) * 3 );
		fin.read( (char*)&quat, sizeof(D3DXQUATERNION) );
		
		D3DXMATRIX& matrix = skeleton->bones[i].model2bone;
		D3DXMatrixRotationQuaternion( &matrix, &quat );
		//D3DXMatrixTranspose( &matrix, &matrix );
	
		matrix._41 = trans[0];
		matrix._42 = trans[1];
		matrix._43 = trans[2];
	}
	//////////////////////////////////// load animations /////////////////////////////////////////
	fin.read( anim_name, 32 );
	fin.read( (char*)&duration, sizeof(float) );
	Track* track;
	tracks.resize( num_bones );
	for ( int j = 0; j < num_bones; ++j )
	{
		int num_kframes;
		track = &tracks[j];
		fin.read( (char*)&num_kframes, sizeof(int) );
		track->num_kframes = num_kframes;
		track->kframes = new KeyFrame[num_kframes];
		fin.read( (char*)track->kframes, sizeof(KeyFrame)*num_kframes );
	}
}
//==========================================================================
int Animation::BuildMatrices( AnimationController* animctrl, D3DXMATRIX* matrices )
{
	int play = 1;
	if ( duration < animctrl->anim_time )
	{
		if ( animctrl->looping ) 
		{
			ZeroMemory( animctrl->track_data, sizeof(int) * MAX_BONES );
			animctrl->anim_time = 0.0f;
		}
		else 
		{
			play = 0;
			animctrl->anim_time = duration;
		}
	}
	
	Bone* bones = skeleton->bones;
	Track* track;
	D3DXQUATERNION rot;
	float trans[3];
	int k;
	float time = animctrl->anim_time;
	
	for ( int i = 0; i < skeleton->num_bones; ++i )
	{
		track = &tracks[i];
		if ( track->num_kframes == 0 )
		{
			D3DXMatrixIdentity( matrices + i );
		}
		else
		{
			k = animctrl->track_data[i];
			while ( k < track->num_kframes && time > track->kframes[k].time ) ++k;
			if ( k == track->num_kframes || k == 0 )
			{
				if ( k == track->num_kframes ) --k;
				memcpy( &rot, track->kframes[k].rot, sizeof(D3DXQUATERNION) );
				memcpy( trans, track->kframes[k].trans, sizeof(float) * 3 );
			}
			//////////////////////////////////////// interpolating ///////////////////////////////////////////
			else
			{
				--k;
				KeyFrame* k0 = &track->kframes[k], *k1 = &track->kframes[k+1];
				float t = time - k0->time;
				t /= k1->time - k0->time;
				D3DXQuaternionSlerp( &rot, (D3DXQUATERNION*)k0->rot, (D3DXQUATERNION*)k1->rot, t );
				trans[0] = t * ( k1->trans[0] - k0->trans[0] ); trans[0] += k0->trans[0];
				trans[1] = t * ( k1->trans[1] - k0->trans[1] ); trans[1] += k0->trans[1];
				trans[2] = t * ( k1->trans[2] - k0->trans[2] ); trans[2] += k0->trans[2];
			}
			D3DXMatrixRotationQuaternion( matrices + i, &rot );
			(matrices + i)->_41 = trans[0];
			(matrices + i)->_42 = trans[1];
			(matrices + i)->_43 = trans[2];

			animctrl->track_data[i] = k;
			if ( skeleton->bones[i].parent > -1 )
			{
				D3DXMatrixMultiply( matrices + i, matrices + i, matrices + skeleton->bones[i].parent );
			}
			
		}// end if ( num_kframes == 0 ) else
	}

	for ( int i = 0; i < skeleton->num_bones; ++i )
	{
		D3DXMatrixMultiply( matrices + i, &skeleton->bones[i].model2bone, matrices + i );
		D3DXMatrixTranspose( matrices + i, matrices + i );
	}
	return play;
}
//==========================================================================
void Animation::Release()
{
	if ( skeleton ) delete skeleton; 
	skeleton = NULL;
	delete this;
}

}