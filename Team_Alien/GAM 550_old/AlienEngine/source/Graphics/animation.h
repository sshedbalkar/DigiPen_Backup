#pragma once

namespace Framework
{
#define MAX_BONES					80

class AnimationController;

struct Bone
{
	int parent;
	D3DXMATRIX model2bone;
	//int sibling;
	//int child;
};

struct Skeleton
{
	int num_bones;
	Bone* bones;
	Skeleton();
	~Skeleton();
};

//A Keyframe is a snapshot of the node at a point in time.
//When rendering the game will interpolate between
//keyframes to create smooth motion.
struct KeyFrame
{
	float time;
	float trans[3];
	float rot[4];
};

//A Track or Path is the set of keyframes that correspond to a particular node.
//Each track can have a different number of keyframe and the keyframes 
//do not have to be distributed on regular intervals.
struct Track
{
	int num_kframes;
	KeyFrame* kframes;
	Track();
	~Track();
};

//An animation is a set of tracks
struct Animation
{
	Skeleton* skeleton;
	//Duration of the Animation in Seconds
	float duration;
	std::vector<Track> tracks;

	void Load( std::ifstream& fin );
	int BuildMatrices( AnimationController* animctrl, D3DXMATRIX* matrices );
	void Release();

	Animation();
private:
	~Animation(){}
};

}