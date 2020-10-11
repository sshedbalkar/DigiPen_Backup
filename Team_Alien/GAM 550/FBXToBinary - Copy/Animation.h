///////////////////////////////////////////////////////////////////////////////
//
//	Animation.h
//	Extracts animation data (keyframes and tracks) from the fbx scene take and 
//	writes it to a binary format.
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ChunkFileWriter.h"
#include "FileElements.h"
#include "Conversion.h"
#include "Utility.h"

//A Keyframe is a snapshot of the node at a point in time.
//When rendering the game will interpolate between
//keyframes to create smooth motion.
struct KeyFrame
{
	float Time;
	FbxVector4 T;
	FbxQuaternion R;
};

//A Track or Path is the set of keyframes that correspond to a particular node.
//Each track can have a different number of keyframe and the keyframes 
//do not have to be distributed on regular intervals.
struct Track
{
	std::vector< KeyFrame > keyFrames;
};

//An animation is a set of tracks
struct Animation
{
	std::string Name;
	//Duration of the Animation in Seconds
	float Duration;
	std::vector< Track > tracks;
};

void WriteAnimation( Animation& animation,ChunkFileWriter& writer);
void ExtractAnimationTracksFromTake(FbxScene* pScene,Animation& anim,Converter& convert,std::vector<FbxNode*> animationNodes,FbxAnimStack* stack );
void ExtractAnimationTrack(FbxNode* pNode,Animation& anim,Converter& converter,int trackIndex,FbxAnimStack* stack , KTime start , KTime stop);
