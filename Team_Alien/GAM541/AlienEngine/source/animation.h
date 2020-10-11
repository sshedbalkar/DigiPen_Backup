#pragma once

#include "VMath.h"
#include <vector>

namespace Framework
{

class Animation
{
	struct Frame
	{
		Frame() {};//: orients(NULL),positions(NULL) {}
		std::vector<Vec4> orients;
		std::vector<Vec3> positions;
		//Vec4* orients;
		//Vec3* positions;
	};
	struct Joint
	{
		Joint() : offspring(-1), sibling(-1) {};
		int offspring;
		int sibling;
	};
	struct FrameBB
	{
		Vec3 min;
		Vec3 max;
	};
	float frametime;
	int num_joints;
	int num_frames;
	Frame* frames;
	FrameBB* frameBBs;

public:
	Animation();
	~Animation();

	void Initialize( const char* filename );
	bool GetJointAnimation( bool looping, float& time, int& n_joints, Vec4** orients, Vec3** positions );
	void Free();

private:
	void SpinSkeleton( int index, int parent, Joint* joints, int f );
};

}//end namespace