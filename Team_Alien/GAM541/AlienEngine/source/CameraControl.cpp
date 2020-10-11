#include "CameraControl.h"
#include "GameLogic.h"
#include "XMLSerialization.h"
#include "Utility.h"
#include "Audio.h"
#include "HeroProperties.h"

namespace Framework
{
	void CameraControl::Initialize()
	{
		std::cout<<"Opening Camera xml file: SmartCamera.xml"<<"\n";
		XMLReader *reader = new XMLReader();
		if(reader->open("Objects\\SmartCamera.xml"))
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
				camData.push_back(arch);
			}
			deletePointersVec(comps);
			delete str;
		}
		delete reader;

	}

	void CameraControl::Update(float dt)
	{
		if(LOGIC->HERO->has(HeroProperties)->AllowMovement)
		{
			Vec3 temp=LOGIC->HERO->has(Transform)->Position;
			LOGIC->CAMERA->has(Transform)->Position.x=temp.x;
			LOGIC->CAMERA->has(Transform)->Position.y=0.0;
			LOGIC->CAMERA->has(Transform)->Position.z=-45.0f;

			for(unsigned int i=0;i<camData.size();i+=3)
			{
				if(camData[i].y==0)
				{
					if(temp.x>=camData[i].x &&  temp.x<=camData[i+1].x)
					{
						if(camData[i+2].x!=0)
							LOGIC->CAMERA->has(Transform)->Position.x=camData[i+2].x;
						if(camData[i+2].y!=0)
							LOGIC->CAMERA->has(Transform)->Position.y=camData[i+2].y;
						if(camData[i+2].z!=0)
							LOGIC->CAMERA->has(Transform)->Position.z=camData[i+2].z;
					}
				}else
				{
					if(temp.x>=camData[i].x &&  temp.x<=camData[i+1].x && temp.y>=camData[i].y &&  temp.y<=camData[i+1].y)
					{
						if(camData[i+2].x!=0)
							LOGIC->CAMERA->has(Transform)->Position.x=camData[i+2].x;
						if(camData[i+2].y!=0)
							LOGIC->CAMERA->has(Transform)->Position.y=camData[i+2].y;
						if(camData[i+2].z!=0)
							LOGIC->CAMERA->has(Transform)->Position.z=camData[i+2].z;
					}
				}
			}
		}
		
		if(LOGIC->CAMERA->has(Transform)->Position.x>=463.0f && LOGIC->CAMERA->has(Transform)->Position.y>= 1)
		{
			if(AUDIO->isPlaying("background"))
			{
				AUDIO->changeVolume("background",AUDIO->findSound("background")->getVolume()-(dt/3.0f));
				if(AUDIO->findSound("background")->getVolume() < 0.0f)
				{
					AUDIO->changeVolume("background",0.001f);
					AUDIO->Stop("background");
				}
			}
			AUDIO->Play ("boss_fight_music");
			float volume=AUDIO->findSound("boss_fight_music")->getVolume();
			if(volume<=0.5)
			{
				AUDIO->changeVolume("boss_fight_music",volume+ dt/5.0f);
			}
		}
		else
		{
			if(AUDIO->isPlaying ("boss_fight_music"))
			{
				AUDIO->changeVolume("boss_fight_music",AUDIO->findSound("boss_fight_music")->getVolume()-(dt/3.0f));
				if(AUDIO->findSound("boss_fight_music")->getVolume() < 0.0f)
				{
					AUDIO->changeVolume("boss_fight_music",0.001f);
					AUDIO->Stop("boss_fight_music");
				}
				AUDIO->Play ("background");
				float volume=AUDIO->findSound("background")->getVolume();
				if(volume<=1.0)
				{
					AUDIO->changeVolume("background",volume+ dt/5.0f);
				}
			}
		}

	}
}