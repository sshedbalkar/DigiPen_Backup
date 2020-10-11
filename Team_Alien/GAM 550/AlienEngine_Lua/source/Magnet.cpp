#include "Magnet.h"
#include "Physics.h"

namespace Framework
{
	void Magnet::Update()
	{
		if(a==NULL || b==NULL) return;
		Vector3 posa,posb;
		posa=a->has(Transform)->Position;
		posb=b->has(Transform)->Position;
		//if(squaredDistance(posa,posb)>200) return;
		Vector3 dir=posb-posa;
		dir.normalise();
		a->has(RigidBody)->addForce(dir*800);
		b->has(RigidBody)->addForce(dir*-800);
	}
}