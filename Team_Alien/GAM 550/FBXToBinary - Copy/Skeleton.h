///////////////////////////////////////////////////////////////////////////////
//
//	Skeleton.h
//	Extracts skeleton data (bone graph and bind pose) from FBX scene and write
//	them in binary format.
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////
#include "Conversion.h"

struct Bone
{
	std::string Name;
	int ParentIndex;
	int BoneIndex;
	FbxNode * BoneNode;

	FbxVector4 BindPos;
	FbxQuaternion BindRot;

	FbxVector4 BoneSpacePos;
	FbxQuaternion BoneSpaceRot;
};

struct Skeleton
{
	Skeleton();
	~Skeleton();
	std::vector<Bone*> Bones;
	void Write(ChunkFileWriter& writer);
};

int GetBoneIndex(FbxNode* pNode,Skeleton& skel);
void ExtractSkeleton(FbxNode* root,Skeleton& skel,std::vector<FbxNode*>& boneNodes);
void ExtractBindPose(FbxNode* pNode,FbxPose* pPose ,Bone& bone ,Converter& converter);
void ExtractBindMatrix(FbxNode* pNode,Bone& bone ,FbxAMatrix& matrix,Converter& converter);



