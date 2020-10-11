///////////////////////////////////////////////////////////////////////////////
//
//	Scene.h
//	Extract data from an FBX scene are writes it to a binary file.
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Conversion.h"
#include "FileElements.h"
#include "Mesh.h"
#include "Skeleton.h"
#include "FilePath.h"
#include "Animation.h"

class Scene
{
public:

	bool ExtractAnimations;
	bool ExtractMesh;
	bool ExtractSkinData;
	bool ExtractSkeletonData;

	Skeleton Skel;
	FbxPose * BindPose;
	std::vector<MeshData*> Meshes;
	std::vector<FbxNode*> BoneNodes;
	std::vector<Animation*> Animations;

	Scene();
	~Scene();

	bool ExtractSceneData(FbxScene* gFBXScene);
	void WriteScene(std::string filename);
	void CollectMeshes(FbxNode* pRootNode,bool onlySkinned);

};