#include "Vector3.h"
namespace Framework
{

std::ostream& operator<<(std::ostream &out, const Vector3 &v)
{
	out<<"["<<v.x<<","<<v.y<<","<<v.z<<"]"<<std::endl;
	return out;
}

}