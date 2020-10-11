#include "Precompiled.h"
#include "ActivateObject.h"
#include "GameLogic.h"
#include "Audio.h"

namespace Framework
{

	void ActivateObject::Update(float dt)
	{
		std::list<GOC*>::iterator  it;
		
		for(it=LOGIC->ListOfButtons.begin();it!=LOGIC->ListOfButtons.end();++it)
		{
			if((*it)->has(Body)->isCollidingUp)
			{
				int id = (*it)->has(InteractiveObjProperties)->GetSecondaryObjID();			//#### added temporary code
				if(m_buttons[id]==0 )
					AUDIO->Play("button");
				m_buttons[id]=1;
			}
			else
			{
				int id = (*it)->has(InteractiveObjProperties)->GetSecondaryObjID();			//#### added temporary code
				m_buttons[id]=0;
			}
			(*it)->has(InteractiveObjProperties)->Update (dt);
		}

		Vec4 temp;
		for(it=LOGIC->ListOfDoorsToBeActivated.begin();it!=LOGIC->ListOfDoorsToBeActivated.end();++it)
		{
			bool open=false;
			temp=(*it)->has(InteractiveObjProperties)->GetObjectActivatedByVec();
			if(m_buttons[(int)temp.x]==1)
				if(m_buttons[(int)temp.y]==1)
					if(m_buttons[(int)temp.z]==1)
						if(m_buttons[(int)temp.w]==1)
							open=true;
							
			(*it)->has(InteractiveObjProperties)->MarkDoorOpen(open);				
			(*it)->has(InteractiveObjProperties)->Update (dt);
		}
	}

}
