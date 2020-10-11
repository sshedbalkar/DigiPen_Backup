#include "Quaternion.h"


namespace Framework
{

std::ostream& operator<<(std::ostream &out, const Quaternion &v)
{
	out<<"["<<v.r<<","<<v.i<<","<<v.j<<","<<v.k<<"]"<<std::endl;
	return out;
}

Quaternion::Quaternion(D3DXQUATERNION &q)
{
	i=q.x;
	j=q.y;
	k=q.z;
	r=q.w;
}

D3DXQUATERNION Quaternion::QuaternionToD3DXQUATERNION()
{
	D3DXQUATERNION Q;
	Q.x=(float)i;
	Q.y=(float)j;
	Q.z=(float)k;
	Q.w=(float)r;
	return Q;
}

}