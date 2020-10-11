#include "Quaternion.h"


namespace Framework
{

std::ostream& operator<<(std::ostream &out, const Quaternion &v)
{
	out<<"["<<v.r<<","<<v.i<<","<<v.j<<","<<v.k<<"]"<<std::endl;
	return out;
}


}