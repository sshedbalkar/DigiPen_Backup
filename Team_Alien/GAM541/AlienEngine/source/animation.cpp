#include "animation.h"
#include "DebugDiagnostic.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace Framework
{

//===================================================
Animation::Animation() : frames(NULL), num_joints(0), num_frames(0), frametime( 1.0f/30.0f )
{
}
//===================================================
Animation::~Animation()
{
	Free();
}
//===================================================
void Animation::Free()
{
	if ( !frames ) return;
	for ( int i = 0; i < num_frames; ++i )
	{
//		delete [] (frames[i].orients);
//		delete [] (frames[i].positions);
	}
	delete [] frames;
	delete [] frameBBs;
}
//===================================================
void Animation::Initialize( const char* filename )
{
	std::ifstream fin( filename );
	ErrorIf( !fin, "cannot open %s.", filename );

	Joint* joints;
	Quat* prev_orients;
	Quat* prev_rots;
	std::string str;
	std::stringstream ss;
	char line[256];
	char c;
	int num;
	int root;

	fin.getline( line, 255 );
	fin.getline( line, 255 );
	fin.getline( line, 255 );
	// now parsing
		//numFrames 50
		//numJoints 5
		//frameRate 24
		//numAnimatedComponents 30
	fin.getline( line, 255 );
	ss.clear(); ss.str(line);
	ss >>str >>num_frames;
	fin.getline( line, 255 );
	ss.clear(); ss.str(line);
	ss >>str >>num_joints;
	fin.getline( line, 255 );
	fin.getline( line, 255 );
	fin.getline( line, 255 );
	// now parsing
		//hierarchy {
		//"name"   parent flags startIndex
		//...
	fin.getline( line, 255 );
	joints = new Joint[num_joints];
	for ( int j = 0; j < num_joints; ++j )
	{
		fin.getline( line, 255 );
		ss.clear(); ss.str(line);
		ss >>str >>num;

		if ( num < 0 ) { root = j; continue;}
		if ( joints[num].offspring == -1 ) 
		{ joints[num].offspring = j; continue; }
		int index = joints[num].offspring;
		while ( joints[index].sibling != -1 ) index = joints[index].sibling;
		joints[index].sibling = j;
	}

	fin.getline( line, 255 );
	fin.getline( line, 255 );
	// now parsing
		//bounds {
		//( min.x min.y min.z ) ( max.x max.y max.z )
		//...
		//}
	fin.getline( line, 255 );
	frameBBs = new FrameBB[ num_frames ];
	for ( int b = 0; b < num_frames; ++b )
	{
		fin.getline( line, 255 );
		ss.clear(); ss.str(line);
		ss >>c>>frameBBs[b].min.x>>frameBBs[b].min.y>>frameBBs[b].min.z>>c
		   >>c>>frameBBs[b].max.x>>frameBBs[b].max.y>>frameBBs[b].max.z;
	}
	fin.getline( line, 255 );
	fin.getline( line, 255 );
	// now parsing
		//baseframe {
		//( pos.x pos.y pos.z ) ( orient.x orient.y orient.z )
		//...
		//}
	fin.getline( line,255 );
	for	( int j = 0; j < num_joints; ++j ) fin.getline( line,255 );
	fin.getline( line,255 );
	fin.getline( line,255 );
	// now parsing
		//frame frameIndex {
		//<float> <float> <float> ...
		//}
	frames = new Frame[num_frames];
	prev_orients = new Quat[num_joints];
	prev_rots = new Quat[num_joints];
	for ( int f = 0; f < num_frames; ++f )
	{
		fin.getline( line, 255 );
//		frames[f].orients = new Vec4[num_joints];
//		frames[f].positions = new Vec3[num_joints];
		//frames[f].orients.resize( num_joints );
		//frames[f].positions.resize( num_joints );
		for ( int j = 0; j < num_joints; ++j )
		{
			fin.getline( line, 255 );
			ss.clear(); ss.str(line);
			Vec3 fpos;
			Vec4 forient;
			ss >>fpos.x >>fpos.y >>fpos.z >>forient.x >>forient.y >>forient.z;
			//ss >>frames[f].positions[j].x>>frames[f].positions[j].y>>frames[f].positions[j].z
			   //>>frames[f].orients[j].x>>frames[f].orients[j].y>>frames[f].orients[j].z;
			
			//float t = 1.0f - (frames[f].orients[j].x * frames[f].orients[j].x) 
			//			   - (frames[f].orients[j].y * frames[f].orients[j].y)
			//			   - (frames[f].orients[j].z * frames[f].orients[j].z);
			//if (t < 0.0f) frames[f].orients[j].w = 0.0f;
			//else frames[f].orients[j].w = -sqrt(t);
			
			float t = 1.0f - (forient.x * forient.x) 
						   - (forient.y * forient.y)
						   - (forient.z * forient.z);
			if (t < 0.0f) forient.w = 0.0f;
			else forient.w = -sqrt(t);

			frames[f].orients.push_back( forient );
			frames[f].positions.push_back( fpos );
		}
		// values loaded, calculate matrices
		SpinSkeleton( root, -1, joints, f );

		fin.getline( line, 255 );
		fin.getline( line, 255 );
	}
	fin.close();
	delete [] joints;
	delete [] prev_orients;
	delete [] prev_rots;

//	std::cout <<num_frames <<" " <<num_joints <<std::endl;
}
//===================================================
bool Animation::GetJointAnimation( bool looping, float& time, int& n_joints, Vec4** orients, Vec3** positions )
{
	bool ret = true;
	if ( time > frametime * num_frames )
	{
		if ( !looping ) ret = false;
		else time -= frametime * num_frames;
	}
	n_joints = num_joints;
	int f = (int)(time / frametime);
	f = f >= num_frames ? num_frames-1 : f;
	*orients = frames[f].orients.data();
	*positions = frames[f].positions.data();
	return ret;
}
//===================================================
void Animation::SpinSkeleton( int index, int parent, Joint* joints, int f )
{
	if ( parent >= 0 )
	{
		QuatRotatePoint( frames[f].orients[parent], frames[f].positions[index], frames[f].positions[index] );
		frames[f].positions[index] += frames[f].positions[parent];
		QuatMultiply( frames[f].orients[index], frames[f].orients[parent], frames[f].orients[index] );
		
		//std::cout <<f<<" "<<index<<"("<<frames[f].positions[index].x<<" "<<frames[f].positions[index].y<<" "<<frames[f].positions[index].z<<")";
		//std::cout <<"("<<frames[f].orients[index].x<<" "<<frames[f].orients[index].y<<" "<<frames[f].orients[index].z<<")";
		//std::cout <<std::endl;

	}

	if ( joints[index].offspring >= 0 )
	{
		SpinSkeleton( joints[index].offspring, index, joints, f );
		int off = joints[index].offspring;
		while ( joints[off].sibling >= 0 )
		{
			off = joints[off].sibling;
			SpinSkeleton( off, index, joints, f );
		}
	}
}

}