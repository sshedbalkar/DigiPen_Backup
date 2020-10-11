#include "Precompiled.h"
#include "EditorFunctionality.h"
#include "Physics.h"
#include "Graphics.h"
#include "WindowsSystem.h"
#include "Factory.h"
#include "Core.h"
#include "VMath.h"
#include "LevelEditor.h"
#include "TweakBarManager.h"
#include "Axis.h"
#include "ModelComponent.h"

namespace Framework
{
	EditorFunctionality::EditorFunctionality()
	{
	}

	EditorFunctionality::~EditorFunctionality()
	{
	}


	void EditorFunctionality::SelectObjects()
	{
		//not done
		real dx,dy,sidex,sidey;
		D3DXMATRIX matrix;
		D3DXMATRIX projectionMatrix;
		matrix=GRAPHICS->GetViewProjMatrix();
		
		GameObjectIdMapType mapOfGOCS = FACTORY->GetAllObjects() ;
		GameObjectIdMapType::iterator p;
		for(p = mapOfGOCS.begin(); p != mapOfGOCS.end(); p++)	
			p->second->selected=false;
		
		real x0 = (((2.0f * (float)EDITOR->clickedPosition.x ) / (float)GRAPHICS->screen_width ) - 1.0f);
		real y0 = ((((2.0f * (float)EDITOR->clickedPosition.y) / (float)GRAPHICS->screen_height) - 1.0f) * -1.0f);		
		real x1 = (((2.0f * (float)EDITOR->unClickedPosition.x ) / (float)GRAPHICS->screen_width ) - 1.0f);
		real y1 = ((((2.0f * (float)EDITOR->unClickedPosition.y) / (float)GRAPHICS->screen_height) - 1.0f) * -1.0f);
			
		if(x0>x1)
		{
			real temp=x0;
			x0=x1;
			x1=temp;
		}
		
		if(y0<y1)
		{
			real temp=y0;
			y0=y1;
			y1=temp;
		}
		Vec3 camPos=GRAPHICS->GetCameraPosition();
		for ( p = mapOfGOCS.begin(); p != mapOfGOCS.end(); ++p )
		{
			Transform* transform = p->second->has( Transform );
			Vec3 pos=p->second->has( Transform )->Position;
			if ( !transform ) continue;
			D3DXVECTOR4 positionWS( transform->Position[0], transform->Position[1], transform->Position[2], 1.0f );
			D3DXVECTOR4 positionPS;
			D3DXVec4Transform( &positionPS, &positionWS, &matrix );
			float x = positionPS.x / positionPS.w, y = positionPS.y / positionPS.w;
			if (  x > x0 && y > y1 && x < x1 && y < y0 ) 
			{
				if((squaredDistance(Vector3(camPos.x,camPos.y,camPos.z),Vector3(pos.x,pos.y,pos.z))<(EDITOR->maxSelectDistance*EDITOR->maxSelectDistance)) &&
					p->second->has(CollisionPrimitive) ){
					EDITOR->selectedObjects.push_back( p->second );
					ModelComponent* pmc = p->second->has(ModelComponent);
					if(pmc) pmc->ChangeTextureDebug();
				}
			}
		}
	}

	GOC* EditorFunctionality::GetGOCFromIntersection(Vector3 & position,Vector3 & direction)
	{
		GOC * Object=NULL;
		Vector3 tempposition;
		Ray ray(position,direction);
		CollisionDetector detect;
		real distance=REAL_MAX;
		GameObjectIdMapType mapOfGOCS = FACTORY->GetAllObjects() ;

		GameObjectIdMapType::iterator p;
		for(p = mapOfGOCS.begin(); p != mapOfGOCS.end(); p++)	
		{
			if(p->second->has(CollisionPrimitive))
			{
				p->second->has(CollisionPrimitive)->calculateInternalsinEditor();
				if(p->second->has(CollisionPrimitive)->Id == CollisionPrimitive::ShapeId::Sphere)
				{
					if(detect.rayCastSphere(*(static_cast<CollisionSphere*>(p->second->has(CollisionPrimitive))),ray,tempposition))
					{
						real a=squaredDistance(position,tempposition);
						if(a<distance)
						{
							distance=a;
							Object=p->second;
						}
					}
				}else if(p->second->has(CollisionPrimitive)->Id == CollisionPrimitive::ShapeId::Box)
				{
					if(detect.rayCastBox(*(static_cast<CollisionBox*>(p->second->has(CollisionPrimitive))),ray,tempposition))
					{
						real a=squaredDistance(position,tempposition);
						if(a<distance)
						{
							distance=a;
							Object=p->second;
						}
					}
				}
			}
		}
		return Object;
	}

	void EditorFunctionality::CopySelectedObjects()
	{
		GOC * newGOC = FACTORY->BuildFromArchetype(EDITOR->m_gocToCopy->archId.c_str() ); 
		Vector3 temp=EDITOR->CenterPos + EDITOR->CenterDir*10;
		newGOC->has(Transform)->Position=Vec3( (float)temp.x, (float)temp.y, (float)temp.z);
		newGOC->has(Transform)->Scale= EDITOR->m_gocToCopy->has(Transform)->Scale;
		newGOC->has(Transform)->QUATERNION=EDITOR->m_gocToCopy->has(Transform)->QUATERNION;
		if(EDITOR->m_gocToCopy->has(CollisionPrimitive)->Id == CollisionPrimitive::ShapeId::Sphere)
		{
			static_cast<CollisionSphere*>(newGOC->has(CollisionPrimitive))->radius=static_cast<CollisionSphere*>(EDITOR->m_gocToCopy->has(CollisionPrimitive))->radius;
		}
		else if(EDITOR->m_gocToCopy->has(CollisionPrimitive)->Id == CollisionPrimitive::ShapeId::Box )
		{
			static_cast<CollisionBox*>(newGOC->has(CollisionPrimitive))->halfSize=static_cast<CollisionBox*>(EDITOR->m_gocToCopy->has(CollisionPrimitive))->halfSize;
		}
		newGOC->Initialize();
	}

	void EditorFunctionality::DeleteSelectedObjects()
	{
		for(int i=0;i<EDITOR->selectedObjects.size();++i)
			EDITOR->selectedObjects[i]->Destroy();
		EDITOR->selectedObjects.clear();


		EDITOR->twbmanager->SetSelectedGOC(NULL);
	}

	
	void EditorFunctionality::UpdatePosition()
	{
		switch(EDITOR->axisObject->selectedAxis)
			{
			case(EDITOR->axisObject->AxisSelection::x ):
			{
				real x=EDITOR->MousePosition.x-EDITOR->prevMousePosition.x;
				for(int i=0;i<EDITOR->selectedObjects.size();++i)
					EDITOR->selectedObjects[i]->has(Transform)->Position.x+= (float)x;
				break;
			}
			case(EDITOR->axisObject->AxisSelection::z):
			{
				real z=(EDITOR->screenMousePosition.x-EDITOR->prevScreenMousePosition.x);
				z/=GRAPHICS->screen_width;
				for(int i=0;i<EDITOR->selectedObjects.size();++i)
					EDITOR->selectedObjects[i]->has(Transform)->Position.z+= (float)(z*5);
				break;
			}
			case(EDITOR->axisObject->AxisSelection::y):
			{
				real y=EDITOR->MousePosition.y-EDITOR->prevMousePosition.y;
				for(int i=0;i<EDITOR->selectedObjects.size();++i)
					EDITOR->selectedObjects[i]->has(Transform)->Position.y+= (float)y;
				break;
			}
			}
	}


	void EditorFunctionality::UpdateRotation()
	{
		if(EDITOR->axisObject->selectedRotationAxis==EDITOR->axisObject->AxisSelection::none)
			return;
		real dist;
		D3DXQUATERNION q;
		if(EDITOR->axisObject->selectedRotationAxis==EDITOR->axisObject->AxisSelection::y)
		{
			dist=EDITOR->MousePosition.x-EDITOR->prevMousePosition.x;

			q=calculateQuaterion(-(float)dist,Vec3(0,1,0));

		}else if(EDITOR->axisObject->selectedRotationAxis==EDITOR->axisObject->AxisSelection::x)
		{
			dist=EDITOR->MousePosition.y-EDITOR->prevMousePosition.y;

			q=calculateQuaterion(-(float)dist,Vec3(1,0,0));

		}else if(EDITOR->axisObject->selectedRotationAxis==EDITOR->axisObject->AxisSelection::z)
		{
			dist=EDITOR->MousePosition.x-EDITOR->prevMousePosition.x;

			q=calculateQuaterion(-(float)dist,Vec3(0,0,1));

		}
		for(int i=0;i<EDITOR->selectedObjects.size();++i)
		{
			EDITOR->selectedObjects[i]->has(Transform)->QUATERNION*=q;
			Quaternion localQ=Quaternion(q.w,q.x,q.y,q.z);
			Vec3 temp= EDITOR->selectedObjects[i]->has(Transform)->Position-EDITOR->center;
			Vector3 dir=temp;
			Vector3 newPos=((localQ*dir)*dir.magnitude())+EDITOR->center;
			EDITOR->selectedObjects[i]->has(Transform)->Position=Vector3ToVec3(newPos);
		}
	}



	

}
