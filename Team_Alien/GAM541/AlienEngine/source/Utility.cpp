#include "Utility.h"
//#include "Physics.h"
//
namespace Framework
{
	void splitString(const std::string& Str, char delimiter, std::vector<std::string>& Result)
	{
		using namespace std;
		string::size_type i = 0;
		string::size_type j = Str.find(delimiter);
		while (j != string::npos)
		{
			Result.push_back(Str.substr(i, j-i));
			i = ++j;
			j = Str.find(delimiter, j);
			if (j == string::npos)
			{
				Result.push_back(Str.substr(i, Str.length( )));
			}
		}
	}
	void readArrayFromString(const std::string& str, std::vector<float>& vec)
	{
		std::vector<std::string> strVec;
		splitString(str, ',', strVec);
		//
		for(std::vector<std::string>::iterator it1 = strVec.begin(), it2 = strVec.end(); it1 != it2; ++it1)
		{
			float f;
			from_string<float>(f, *it1, std::dec);
			vec.push_back(f);
		}
	}
	std::ostream& operator<<(std::ostream &out, const Vec2 &v)
	{
		return out<<"["<<v.x<<", "<<v.y<<"]";
	}
	std::ostream& operator<<(std::ostream &out, const Vec3 &v)
	{
		return out<<"["<<v.x<<", "<<v.y<<", "<<v.z<<"]";
	}
	std::ostream& operator<<(std::ostream &out, const Vec4 &v)
	{
		return out<<"["<<v.x<<", "<<v.y<<", "<<v.z<<", "<<v.w<<"]";
	}
	std::ostream& operator<<(std::ostream &out, const Mat4 &v)
	{
		out<<"[("<<v._11<<", "<<v._12<<", "<<v._13<<", "<<v._14<<"), "
			<<"("<<v._21<<", "<<v._22<<", "<<v._23<<", "<<v._24<<"), "
			<<"("<<v._31<<", "<<v._32<<", "<<v._33<<", "<<v._34<<"), "
			<<"("<<v._41<<", "<<v._42<<", "<<v._43<<", "<<v._44<<")]";
		return out;
	}
	// generates a random float number in a range
	float randomFloat(const float& min, const float& max)
	{
		float r = (float)rand() / (float)RAND_MAX;
		return min + r * (max - min);
	}
	/*
	std::ostream& operator<<(std::ostream &out, const CollisionMap *m)
	{
		return m->print(out);
	}
	std::ostream& operator<<(std::ostream &out, const MessageCollide *m)
	{
		return out<<"{Collision: Normal: "<<m->ContactNormal<<", Impulse: "<<m->Impulse<<" With: Name: "<<m->CollidedWith->getName()<<", Id: "<<m->CollidedWith->getId()<<"}";
	}
	*/
}