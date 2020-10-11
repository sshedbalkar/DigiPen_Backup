///////////////////////////////////////////////////////////////////////////////////////
//
//	Utility.cpp
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Common.h"
#ifdef IOS_REF
	#undef  IOS_REF
	#define IOS_REF (*(pSdkManager->GetIOSettings()))
#endif

bool IsBoneNode(FbxNode* pNode)
{
	if( pNode->GetNodeAttribute() != NULL )
	{
		FbxNodeAttribute::EType lAttributeType = pNode->GetNodeAttribute()->GetAttributeType();
		if( lAttributeType == FbxNodeAttribute::eSkeleton )
			return true;
	}
	return false;
}


//Does homogeneous transform
KFbxVector4 Transform(const FbxAMatrix& pXMatrix, const KFbxVector4& point)
{
	FbxMatrix * m = (FbxMatrix*)&pXMatrix;
	return m->MultNormalize( point );
}

//FBX SDK has two different types of matrices XMatrix and Matrix
//XMatrix is affine and has functions for extracting pos,trans and rot
//Matrix has all the matrix operations and is returned by several SDK functions
//because of this the matrices need to be converted between each other
void SetXMatrix(FbxAMatrix& pXMatrix, const FbxMatrix& pMatrix)
{
	memcpy((double*)pXMatrix.mData, &pMatrix.mData, sizeof(pMatrix.mData));
}

void SetMatrix(FbxMatrix& pXMatrix, const FbxAMatrix& pMatrix)
{
	memcpy((double*)&pXMatrix.mData, &pMatrix.mData, sizeof(pMatrix.mData));
}

// Get the matrix of the node in the given pose
FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
{
	FbxAMatrix lPoseMatrix;
	FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);
	SetXMatrix(lPoseMatrix,lMatrix);
	return lPoseMatrix;
}

// Get the matrix of the node in the given pose
FbxAMatrix GetPoseMatrixInverse(FbxPose* pPose, int pNodeIndex)
{
	FbxAMatrix lPoseMatrix;
	FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);
	lMatrix = lMatrix.Inverse();
	SetXMatrix(lPoseMatrix,lMatrix);
	return lPoseMatrix;
}


// Recursive function to get a node's global default position 
// As a prerequisite, parent node's default local position must be already set 
FbxAMatrix GetGlobalDefaultPosition(FbxNode* node) 
{
	// Stores the local position, global position, and parent's global position
	FbxAMatrix local_position;
	FbxAMatrix global_position;
	FbxAMatrix parent_global_position;

	// Get the translation, rotation, and scaling
	KFbxVector4 T, R, S;
	local_position.SetT(node->LclTranslation.Get());
	local_position.SetR(node->LclRotation.Get());
	local_position.SetS(node->LclScaling.Get());

	// If the node has a parent
	if (node->GetParent())
	{
		parent_global_position = GetGlobalDefaultPosition(node->GetParent());
		global_position = parent_global_position * local_position;
	}
	// Otherwise, we've reached the end of the recursion,
	// so the global position is the local position
	else
	{
		global_position = local_position;
	}

	// Return the global position
	return global_position;
}



FbxAMatrix GetPoseMatrix(FbxNode* pNode,FbxPose* pPose)
{
	int lNodeIndex = pPose->Find(pNode);
	return GetPoseMatrix( pPose , lNodeIndex );
}

// Get the geometry deformation local to a node. It is never inherited by the
// children.
FbxAMatrix GetGeometry(FbxNode* pNode) {
	KFbxVector4 lT, lR, lS;
	FbxAMatrix lGeometry;

	lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	lGeometry.SetT(lT);
	lGeometry.SetR(lR);
	lGeometry.SetS(lS);

	return lGeometry;
}

void InitializeSdkObjects(FbxManager*& pSdkManager, FbxScene*& pScene)
{
	// The first thing to do is to create the FBX SDK manager which is the 
	// object allocator for almost all the classes in the SDK.
	pSdkManager = FbxManager::Create();

	if (!pSdkManager)
	{
		Logger::Log( MsgError ,"Unable to create the FBX SDK manager\n");
		exit(0);
	}

	// Create the entity that will hold the scene.
	pScene = FbxScene::Create(pSdkManager,"");

}

void DestroySdkObjects(FbxManager* pSdkManager)
{
	// Delete the FBX SDK manager. All the objects that have been allocated 
	// using the FBX SDK manager and that haven't been explicitly destroyed 
	// are automatically destroyed at the same time.
	if (pSdkManager) pSdkManager->Destroy();
	pSdkManager = NULL;
}

bool LoadScene(FbxManager* pSdkManager, KFbxDocument* pScene, const char* pFilename)
{
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor,  lSDKMinor,  lSDKRevision;
	int lFileFormat = -1;
	bool lStatus;

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	KFbxImporter* lImporter = KFbxImporter::Create(pSdkManager,"");

	
    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename, -1, pSdkManager->GetIOSettings() );

    // Get the version number of the FBX file format.
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);


	// Initialize the importer by providing a filename.
	//lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if( !lImportStatus )
	{
		Logger::Log(MsgError,"Call to KFbxImporter::Initialize() failed.\n");
		Logger::Log(MsgError,"Error returned: %s\n\n", lImporter->GetLastErrorString());

		if (lImporter->GetLastErrorID() == FbxIOBase::eFileVersionNotSupportedYet ||
			lImporter->GetLastErrorID() == FbxIOBase::eFileVersionNotSupportedAnymore)
		{
			Logger::Log(MsgError,"FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			Logger::Log(MsgError,"FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	Logger::Log(MsgStd,"FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		Logger::Log(MsgStd,"FBX version number for file %s is %d.%d.%d\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	Logger::Log(MsgStd,"Importing the Scene...\n");
	// Import the scene.
	lStatus = lImporter->Import(pScene);

	if(lStatus == false)
	{		
		Logger::Log(MsgError,"Failed to import scene.\n");
		Logger::Log(MsgError,"Error returned: %s\n\n", lImporter->GetLastErrorString());
	}
	else
	{
		Logger::Log(MsgResult,"Scene Successfully Imported.\n");
	}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}



FbxNodeAttribute::EType GetNodeAttributeType(FbxNode* pNode)
{
	FbxNodeAttribute* nodeAtt = pNode->GetNodeAttribute();
	if( nodeAtt != NULL )
		return nodeAtt->GetAttributeType();
	return FbxNodeAttribute::eNull;
}

inline bool IsEqualEpsilon( float A, float B )
{
	return fabs( A - B ) <= 1e-5f;
}

bool CheckPositive(KFbxVector4 scale)
{
	if( scale[0] < 0.0f || 
		scale[1] < 0.0f ||
		scale[2] < 0.0f )
	{
		return false;
	}

	return true;
}

bool CheckScaling(KFbxVector4 scale)
{
	if( !IsEqualEpsilon( scale[0] , scale[1] ) ||
		!IsEqualEpsilon( scale[1], scale[2] ) ||
		!IsEqualEpsilon( scale[0], scale[2] ) )
	{
		return false;
	}

	return true;
}

