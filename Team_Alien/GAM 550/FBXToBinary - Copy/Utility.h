///////////////////////////////////////////////////////////////////////////////////////
//
//	Utility.h
//	Various utlity functions some from the FBX samples.
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#pragma once

template< typename type >
void ConvetToStl( std::vector<type>& container , FbxLayerElementArrayTemplate<type>* FbxContainter)
{
	int numberofObjects = FbxContainter->GetCount();
	void * pData = FbxContainter->GetLocked();

	container.resize( numberofObjects );
	memcpy( &container[0] , pData , sizeof(type)*numberofObjects );

	FbxContainter->Release( &pData );
}

template< typename type >
void FillStl( std::vector<type>& container , std::size_t size)
{
	container.resize( size );
	for( std::size_t i=0;i<size;++i)
		container[i] = i;
}

template< typename type>
static inline void DeleteIt( type* pointer ){delete pointer;}

template<typename keytype,typename type>
static inline void DeleteIt(std::pair<const keytype,type>& entry ){delete entry.second;}

template<typename contype>
void DeleteObjectsInContainer( contype& container )
{
	contype::iterator it = container.begin();
	contype::iterator end = container.end();
	for (;it!=end;++it) DeleteIt( *it );
	container.clear();
}

FbxVector4 Transform(const FbxAMatrix& pXMatrix, const FbxVector4& point);
void SetXMatrix(FbxAMatrix& pXMatrix, const FbxMatrix& pMatrix);
void SetMatrix(FbxMatrix& pXMatrix, const FbxAMatrix& pMatrix);
bool IsBoneNode(FbxNode* pNode);
FbxAMatrix GetGeometry(FbxNode* pNode);
FbxAMatrix GetGlobalDefaultPosition(FbxNode* node);

//File Utility
void InitializeSdkObjects(FbxManager*& pSdkManager, FbxScene*& pScene);
void DestroySdkObjects(FbxManager* pSdkManager);
bool LoadScene(FbxManager* pSdkManager, FbxDocument* pScene, const char* pFilename);

FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex);
FbxAMatrix GetPoseMatrix(FbxNode* pNode,FbxPose* pPose);

FbxNodeAttribute::EType GetNodeAttributeType(FbxNode* pNode);

//Check to see if scaling is uniform
bool CheckScaling(KFbxVector4 scale);
//Check to see if scaling if positive
bool CheckPositive(KFbxVector4 scale);
