#include "Precompiled.h"
#include "Axis.h"
#include "Physics.h"
#include "Factory.h"
#include "VMath.h"
#include "LevelEditor.h"


namespace Framework
{

	Axis::Axis()
	{
		
	}

	Axis::~Axis()
	{
	}

	
	void Axis::Initialize()
	{
		xaxis=FACTORY->CreateEmptyComposition();
		Transform *t1=new Transform();
		t1->Scale=Vec3(1.5f,0.1f,0.1f);
		xaxis->AddComponent(CT_Transform,t1);
		CollisionBox *box1=new CollisionBox();
		box1->halfSize=Vector3(0.75f,0.05f,0.05f);
		xaxis->AddComponent(CT_CollisionPrimitive,box1);
		xaxis->Initialize();

		yaxis=FACTORY->CreateEmptyComposition();
		Transform *t2=new Transform();
		t2->Scale=Vec3(0.1f,1.5f,0.1f);
		yaxis->AddComponent(CT_Transform,t2);
		CollisionBox *box2=new CollisionBox();
		box2->halfSize=Vector3(0.05f,0.75f,0.05f);
		yaxis->AddComponent(CT_CollisionPrimitive,box2);
		yaxis->Initialize();

		zaxis=FACTORY->CreateEmptyComposition();
		Transform *t3=new Transform();
		t3->Scale=Vec3(0.1f,0.1f,1.5f);
		zaxis->AddComponent(CT_Transform,t3);
		CollisionBox *box3=new CollisionBox();
		box3->halfSize=Vector3(0.05f,0.05f,0.75f);
		zaxis->AddComponent(CT_CollisionPrimitive,box3);
		zaxis->Initialize();
	}
	
	bool Axis::CheckAxes()
	{
		selectedAxis=none;
		xaxis->has(CollisionPrimitive)->calculateInternalsinEditor();
		yaxis->has(CollisionPrimitive)->calculateInternalsinEditor();
		zaxis->has(CollisionPrimitive)->calculateInternalsinEditor();
		CollisionDetector detect;
		Ray ray(EDITOR->position,EDITOR->direction);
		Vector3 tempposition;
		if(detect.rayCastBox(*(static_cast<CollisionBox*>(xaxis->has(CollisionPrimitive))),ray,tempposition))
		{
			selectedAxis=x;
			return true;
		}else if(detect.rayCastBox(*(static_cast<CollisionBox*>(yaxis->has(CollisionPrimitive))),ray,tempposition))
		{
			selectedAxis=y;
			return true;
		}else if(detect.rayCastBox(*(static_cast<CollisionBox*>(zaxis->has(CollisionPrimitive))),ray,tempposition))
		{
			selectedAxis=z;
			return true;
		}
		return false;
	}

	void Axis::CheckRotationaxes()
	{
		selectedRotationAxis=none;
		Vector3 c(EDITOR->center.x,EDITOR->center.y,EDITOR->center.z);
		Ray r(EDITOR->position,EDITOR->direction);
		{
			Torus t(c,2,0.1,Quaternion(0.7071,0,0.7071,0));
			if(t.intersection(r))
			{
				selectedRotationAxis=y;
				return;
			}
		}
		{
			Torus t(c,2,0.1,Quaternion(0.7071,0.7071,0,0));
			if(t.intersection(r))
			{
				selectedRotationAxis=z;
				return;
			}
		}
		{
			Torus t(c,2,0.1,Quaternion(0.7071,0,0,0.7071));
			if(t.intersection(r))
			{
				selectedRotationAxis=x;
				return;
			}
		}

	}
	
	

	void Axis::Update(float dt)
	{
		switch(EDITOR->state)
		{
		case EDITOR->EditorLevelState::select:
			{
				//select code
				xaxis->has(Transform)->Position=Vec3(9999,9999,9999);
				yaxis->has(Transform)->Position=Vec3(9999,9999,9999);
				zaxis->has(Transform)->Position=Vec3(9999,9999,9999);
				break;
			}
		case  EDITOR->EditorLevelState::move:
			{
				//movement code
				xaxis->has(Transform)->Position=Vec3(EDITOR->center.x+0.75f,EDITOR->center.y,EDITOR->center.z);
				yaxis->has(Transform)->Position=Vec3(EDITOR->center.x,EDITOR->center.y+0.75f,EDITOR->center.z);
				zaxis->has(Transform)->Position=Vec3(EDITOR->center.x,EDITOR->center.y,EDITOR->center.z+0.75f);
				break;
			}
		case  EDITOR->EditorLevelState::rotate:
			{
				//rotate code
				break;
			}
		case  EDITOR->EditorLevelState::scale:
			{
				break;
			}
		}
	}
	

}
