///////////////////////////////////////////////////////////////////////////////////////
//
//	Animation.cpp
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Common.h"
#include "Animation.h"


void CollectKeyTimes( std::set<KTime>& keyTimes,FbxAnimLayer* layer,FbxPropertyT<fbxDouble3>& attribute,const char * curveName)
{		
	//Get all the keyframes for this control curve
	FbxAnimCurve * curve = attribute.GetCurve( layer, curveName );
	if( curve )
	{
		int keyCount = curve->KeyGetCount();
		for( int i=0;i<keyCount;++i)
		{
			keyTimes.insert( curve->KeyGetTime(i) );
		}
	}
}

void ExtractAnimationTracksFromTake(FbxScene* pScene,Animation& anim,Converter& convert,std::vector<FbxNode*> animNodes,FbxAnimStack* stack )
{
	Logger::Log( MsgStd , "Extracting Animation for Take '%s'\n" , stack->GetName() );

	//TODO: Usually this name is meaningless, so you could use the name of the file instead
	anim.Name = stack->GetName();
	
//	FbxAnimStack* stack = FbxCast<FbxAnimStack>(pScene->GetSrcObject(FBX_TYPE(FbxAnimStack), i ) );
	
	//pScene->SetCurrentTake(takeName);
	//KString strTakeName(takeName);
	KTime gStart,gStop;

	FbxTimeSpan time_span = stack->GetLocalTimeSpan();
	//KFbxTakeInfo* pCurrentTakeInfo = pScene->GetTakeInfo(strTakeName);
	gStart = time_span.GetStart();
	gStop = time_span.GetStop();
	KTime duration = gStop - gStart;
	anim.Duration = duration.GetSecondDouble();

	anim.tracks.resize( animNodes.size() );
	for( int i=0;i<animNodes.size();++i)
		ExtractAnimationTrack( animNodes[i] , anim , convert , i , stack , gStart , gStop );

		
	Logger::Log( MsgResult , "Extracted Animation '%s' Length %.2f seconds. Number of Tracks %d\n" , stack->GetName() , anim.Duration , anim.tracks.size() );


}

FbxAMatrix GetLocalMatrixFromTime(FbxNode* pNode,KTime& current)
{
	//We should be extracting the local transform data directly from the node
	//buy this does not work due to other transforms like PreRotation
	//the simplest what to extract the data is to use GetGlobalFromCurrentTake
	//which always returns the correct matrix then use the parent's inverse to
	//get the local matrix (slower but effective)

	//This is what we want to do but it does not work in all cases
	//KFbxVector4 Rotate = pNode->GetLocalRFromCurrentTake( current );
	//KFbxVector4 Trans = pNode->GetLocalTFromCurrentTake( current );
	//KFbxVector4 Scale = pNode->GetLocalSFromCurrentTake( current );

	FbxNode* pParent = pNode->GetParent();
	FbxAMatrix localXForm;
	if( IsBoneNode(pParent) )
	{
		//compute the local transform by getting the global of this
		//node and the parent then multiplying the this nodes global
		//transform by the inverse of the parents global transform
		FbxAMatrix PX = pParent->EvaluateGlobalTransform(current);
		FbxAMatrix LX = pNode->EvaluateGlobalTransform(current);
		PX = PX.Inverse();
		localXForm = PX * LX;
	}
	else
	{
		//We want root bones in global space
		localXForm = pNode->EvaluateGlobalTransform(current);
	}

	return localXForm;
}

void ExtractAnimationTrack(FbxNode* pNode,Animation& anim,Converter& converter,int trackIndex,FbxAnimStack* stack, KTime start , KTime stop)
{

	Logger::Log( MsgDetail , "Reading KeyFrames For Node '%s'\n" , pNode->GetName() );

	Track& track = anim.tracks[ trackIndex ];

	//Collect all the times that a keyframe occurs. This implementation combines
	//all the curves (position,rotation,scale) together into one track for each node

	std::set< KTime > keyTimes;
	keyTimes.insert( start );//Make sure there at least one keyframe

	FbxAnimLayer* layer = stack->GetMember( FBX_TYPE(FbxAnimLayer), 0 );

	CollectKeyTimes( keyTimes , layer ,  pNode->LclRotation , FBXSDK_CURVENODE_COMPONENT_X );
	CollectKeyTimes( keyTimes , layer ,  pNode->LclRotation , FBXSDK_CURVENODE_COMPONENT_Y );
	CollectKeyTimes( keyTimes , layer ,  pNode->LclRotation , FBXSDK_CURVENODE_COMPONENT_Z );

	CollectKeyTimes( keyTimes , layer ,  pNode->LclTranslation , FBXSDK_CURVENODE_COMPONENT_X );
	CollectKeyTimes( keyTimes , layer ,  pNode->LclTranslation , FBXSDK_CURVENODE_COMPONENT_Y );
	CollectKeyTimes( keyTimes , layer ,  pNode->LclTranslation , FBXSDK_CURVENODE_COMPONENT_Z );

	//Make space for all the keyframes
	track.keyFrames.resize( keyTimes.size() );

	Logger::Log( MsgDetail , "Number of Keyframes:%d\n" , keyTimes.size() );

	//Iterate through the keyframe set storing the key frame data for each time
	int keyIndex = 0;	
	std::set<KTime>::iterator it = keyTimes.begin();
	for(;it!=keyTimes.end();++it,++keyIndex)
	{
		KTime current = *it;

		FbxAMatrix localXForm = GetLocalMatrixFromTime(pNode,current);

		//Convert the matrix into the destination coordinate space
		converter.ConvertMatrix( localXForm );

		KFbxVector4 scale = localXForm.GetS();

		if( !CheckScaling(scale) )
		{
			Logger::Log( MsgError , "Non-uniform scaling on node '%s' on take '%s'.\n", pNode->GetName() , stack->GetName() );
			localXForm.SetS( KFbxVector4(1,1,1) );
		}

		track.keyFrames[ keyIndex ].Time = current.GetSecondDouble();
		track.keyFrames[ keyIndex ].T = localXForm.GetT();//Store translation
		track.keyFrames[ keyIndex ].R = localXForm.GetQ();//Store rotation
	}

}




void WriteAnimation( Animation& animation, ChunkFileWriter& writer )
{
	u32 animStartChunk = writer.StartChunk( MarkAnimation );
	writer.Write( f32(animation.Duration) );
	writer.Write( u32(animation.tracks.size()) );
	for( uint t=0;t<animation.tracks.size();++t)
	{
		Track& track = animation.tracks[t];
		writer.Write( u32(track.keyFrames.size()) );
		//printf( "%d\n", track.keyFrames.size() );
		for( uint f=0;f<track.keyFrames.size();++f)
		{
			KeyFrame& keyFrame = track.keyFrames[f];

			writer.Write( f32(keyFrame.Time) );

			writer.Write( f32(keyFrame.T[0]) );
			writer.Write( f32(keyFrame.T[1]) );
			writer.Write( f32(keyFrame.T[2]) );

			writer.Write( f32(keyFrame.R[0]) );
			writer.Write( f32(keyFrame.R[1]) );
			writer.Write( f32(keyFrame.R[2]) );
			writer.Write( f32(keyFrame.R[3]) );	
		}
	}

	writer.EndChunk( animStartChunk );

}

