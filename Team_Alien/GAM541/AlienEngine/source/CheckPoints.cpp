#include "CheckPoints.h"
#include "GameLogic.h"
#include "XMLSerialization.h"
#include "Utility.h"
#include "HeroProperties.h"

namespace Framework
{
	void CheckPoints::Initialize()
	{
		std::cout<<"Opening CheckPoints xml file: CheckPoints.xml"<<"\n";
		XMLReader *reader = new XMLReader();
		if(reader->open("Objects\\CheckPoints.xml"))
		{
			ISerializer* str = reader->data().clone();
			str->readProperty("Points", *str);
			//
			std::vector<ISerializer*> comps;
			str->readProperty("Point", comps);
			//
			for(std::vector<ISerializer*>::iterator it1 = comps.begin(), it2 = comps.end(); it1 != it2; ++it1)
			{
				ISerializer* stream = *it1;
				Vec3 arch;
				stream->readValue("value", arch);
				//std::cout<<arch<<std::endl;
				checkPData.push_back(arch);
			}
			deletePointersVec(comps);
			delete str;
		}
		delete reader;

		
	}

	void CheckPoints::Load()
	{
		Vec3 temp=LOGIC->HERO->has(Body)->Position;
		for(unsigned int i=0;i<checkPData.size();i+=3)
		{
			if(checkPData[i].y==0)
			{
				if(temp.x>=checkPData[i].x &&  temp.x<=checkPData[i+1].x)
				{
					if(checkPData[i+2].x!=0)
						LOGIC->HERO->has(Body)->Position.x=checkPData[i+2].x;
					if(checkPData[i+2].y!=0)
						LOGIC->HERO->has(Body)->Position.y=checkPData[i+2].y;
					//if(checkPData[i+2].z!=0)
						//LOGIC->CAMERA->has(Body)->Position.z=checkPData[i+2].z;
				}
			}else
			{
				if(temp.x>=checkPData[i].x &&  temp.x<=checkPData[i+1].x && temp.y>=checkPData[i].y &&  temp.y<=checkPData[i+1].y)
				{
					if(checkPData[i+2].x!=0)
						LOGIC->HERO->has(Body)->Position.x=checkPData[i+2].x;
					if(checkPData[i+2].y!=0)
						LOGIC->HERO->has(Body)->Position.y=checkPData[i+2].y;
					//if(checkPData[i+2].z!=0)
					//	LOGIC->CAMERA->has(Body)->Position.z=checkPData[i+2].z;
				}
			}
		}
	
		for (std::list<GOC*>::iterator it=LOGIC->ListOfEnemies.begin();it!=LOGIC->ListOfEnemies.end();++it) {
			if ((*it)->has(InteractiveObjProperties)->ResetOnLoad==1)
				(*it)->has(Body)->Position=(*it)->has(Body)->InitialPosition;// open or close door
		}

		for (std::list<GOC*>::iterator it=LOGIC->ListOfBoxes.begin();it!=LOGIC->ListOfBoxes.end();++it) {
				(*it)->has(Body)->Position=(*it)->has(Body)->InitialPosition;// open or close door
				(*it)->has(Body)->Velocity.x=0.0f;
		}
		if (LOGIC->FINALBOSS)
		{
			 LOGIC->FINALBOSS->has(Body)->Position=LOGIC->FINALBOSS->has(Body)->InitialPosition;
		}
		
	}
}