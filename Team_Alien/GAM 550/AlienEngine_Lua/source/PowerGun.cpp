#include "PowerGun.h"
#include "WindowsSystem.h"
#include "GameLogic.h"

namespace Framework
{

	void PowerGun::SendMessage(Message *m)
	{
		switch( m->MessageId )
		{

		case Mid::CharacterKey:
			{
				MessageCharacterKey * key = (MessageCharacterKey*)m;
				if(key->character=='1')
				{
					m_state=powerConnect;
				}
				else if(key->character=='2')
				{
					/*m_state=powerBomb;
					Bomb b;
					b.pos=Vector3(-9,-17,-120);
					b.distance=200;
					b.force=40000;
					bombs.AddBomb(b);*/
				}
				else if(key->character=='3')
				{
					m_state=powerMagnet;
				}
				break;
			}
		case Mid::VirtualKey:
			{
				break;
			}
		case Mid::MouseMove:
			{
				break;
			}
		case Mid::MouseWheel :
			{
				break;
			}
		case Mid::RMouseButton:
			{
				RMouseButton * rmouse = (RMouseButton*)m;
				if(rmouse->ButtonIsPressed)
				{
					if(m_state==powerMagnet)
						magnet.AddBody2(LOGIC->objectPointedAt);	
					else if (m_state==powerConnect)
						connector.AddBody2(LOGIC->objectPointedAt);	
				}
				else
				{
					
				}
				break;
			}
		case Mid::LMouseButton:
			{
				LMouseButton * lmouse = (LMouseButton*)m;
				if(lmouse->ButtonIsPressed)
				{
					if(m_state==powerMagnet)
						magnet.AddBody1(LOGIC->objectPointedAt);	
					else if (m_state==powerConnect)
						connector.AddBody1(LOGIC->objectPointedAt);	
				}
				else
				{
					
				}
				break;
			}
		}
	}



	void PowerGun::Update()
	{
		magnet.Update();
		bombs.Update ();
		connector.Update();
	}

}