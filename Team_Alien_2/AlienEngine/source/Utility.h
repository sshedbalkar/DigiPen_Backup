////////////////////////////////////////////////////////////////
//
//	Utility.h
//	defines various utility functions
//
//	Author: Santosh Shedbalkar
//	Copyright 2011, Digipen Institute of Technology
//
////////////////////////////////////////////////////////////////
#ifndef UTILITY_H
#define UTILITY_H
//#pragma message("Compiling: "__FILE__)
//
#include <sstream>
#include "Containers.h"
#include "VMath.h"
//#include "CollisionMap.h"
//
namespace Framework
{
//#pragma message("Including Class: Utility")
	//std::vector<std::string>;
	class ISerializer;
	class MessageCollide;
	//
	template<typename RefType>
	inline void safeRelease(RefType& interfacePtr)
	{
		if(interfacePtr)
		{
			interfacePtr->Release();
		}
		interfacePtr = NULL;
	}
	//
	template<typename RefType>
	inline void safeDelete(RefType& objectPtr)
	{
		if(objectPtr)
		{
			delete objectPtr;
		}
		objectPtr = NULL;
	}
	//
	void splitString(const std::string& Str, char delimiter, std::vector<std::string>& Result);
	
	template <class T>
	bool from_string(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&))
	{
		std::istringstream iss(s);
		return !(iss >> f >> t).fail();
	}
	//
	void readArrayFromString(const std::string& str, std::vector<float>& vec);
	std::ostream& operator<<(std::ostream &out, const Vec2 &v);
	std::ostream& operator<<(std::ostream &out, const Vec3 &v);
	std::ostream& operator<<(std::ostream &out, const Vec4 &v);
	std::ostream& operator<<(std::ostream &out, const Mat4 &v);
	//std::ostream& operator<<(std::ostream &out, const CollisionMap *m);
	//std::ostream& operator<<(std::ostream &out, const MessageCollide *m);
	//
	template<class T>
	void deletePointersVec(std::vector<T*>& Vec)
	{
		for(std::vector<T*>::iterator it1 = Vec.begin(), it2 = Vec.end(); it1 != it2; ++it1)
		{
			delete *it1;
		}
	}
	//
	// generates a random float number in a range
	float randomFloat(const float& min, const float& max);
}
//
#endif