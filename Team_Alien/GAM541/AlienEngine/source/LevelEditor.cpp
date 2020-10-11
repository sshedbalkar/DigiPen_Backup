#include "Precompiled.h"
#include "LevelEditor.h"
#include "GameLogic.h"
#include "WindowsSystem.h"
#include "Utility.h"
#include "debugGeometry.h"
#include "scriptManager.h"
#include "scriptRegistrator.h"
#include "GSM.h"
#include "LoadLevelClass.h"

namespace Framework
{

	void LevelEditor::Update(float dt)
	{
		static_cast<void>(dt);
			if(m_enabled)
			{//edit state
				if( m_bodymove && GrabbedObject!=NULL)
				{
					GrabbedObject->has(Body)->SetPosition(BodyMouseDiff+LOGIC->WorldMousePosition);
					GrabbedObject->has(Transform)->Position=Vec3(GrabbedObject->has(Body)->Position.x,GrabbedObject->has(Body)->Position.y,0);
				}else if(m_screenMove)
				{
					Vec2 temp=BodyMouseDiff-LOGIC->WorldMousePosition;
					BodyMouseDiff=LOGIC->WorldMousePosition;
					LOGIC->CAMERA->has(Transform)->Position+=Vec3(temp.x/2,temp.y/2,0.0);
				}
				if(m_EditScale)
				{
					if(GrabbedObject!=NULL)
					{
						if(IsCtrlHeld())
						{
							Vec2 temp=(LOGIC->WorldMousePosition-ScaleVec);
							ScaleVec=LOGIC->WorldMousePosition;
							GrabbedObject->has(Transform)->Scale.x+=temp.x;
							GrabbedObject->has(Body)->BodyScale.x+=(temp.x/2.0f);
						}else if(IsAltHeld())
						{
							Vec2 temp=(LOGIC->WorldMousePosition-ScaleVec);
							ScaleVec=LOGIC->WorldMousePosition;
							GrabbedObject->has(Transform)->Scale.y+=temp.y;
							GrabbedObject->has(Body)->BodyScale.y+=(temp.y/2.0f);
						}else
						{
							Vec2 temp=(LOGIC->WorldMousePosition-ScaleVec);
							ScaleVec=LOGIC->WorldMousePosition;
							GrabbedObject->has(Transform)->Scale+=Vec3(temp.x,temp.y,0);
							GrabbedObject->has(Body)->BodyScale+=(temp/2.0);
						}
						static_cast<ShapeAAB*>(GrabbedObject->has(Body)->BodyShape)->Extents=GrabbedObject->has(Body)->BodyScale;
						GrabbedObject->has(DebugGeometry)->ModifyDebugShape("edit", Vec3(GrabbedObject->has(Body)->BodyScale.x*2,GrabbedObject->has(Body)->BodyScale.y*2,0) , Vec4(1,0,0,1));
					}
				}
			}
	}

	void LevelEditor::SendMessageA(Message * m)
	{
		if (m_enabled)
		{
			switch( m->MessageId )
			{
			// The user has pressed a (letter/number) key, we may respond by creating
			// a specific object based on the key pressed.
			case Mid::CharacterKey:
				{
					// Cast to the derived message type
					MessageCharacterKey * key = (MessageCharacterKey*)m;
					//std::cout<<"Got char key: "<<key->character<<"\n";
					//When different keys are pressed create different objects
					if( key->character == 'b' )
					{
						if(GrabbedObject!=NULL)
						{
							std::cout<<"curerent body scale  = "<<GrabbedObject->has(Body)->BodyScale<<std::endl;
							std::cout<<"Enter new body scale  =  ";
							std::cin>>GrabbedObject->has(Body)->BodyScale.x>>GrabbedObject->has(Body)->BodyScale.y;
							static_cast<ShapeAAB*>(GrabbedObject->has(Body)->BodyShape)->Extents=GrabbedObject->has(Body)->BodyScale;
							GrabbedObject->has(DebugGeometry)->ModifyDebugShape("edit", Vec3(GrabbedObject->has(Body)->BodyScale.x*2,GrabbedObject->has(Body)->BodyScale.y*2,0) , Vec4(1,0,0,1));
						}
					}else if( key->character == 'n' )
					{
						if(GrabbedObject!=NULL)
						{
							std::cout<<"curerent scale  = "<<GrabbedObject->has(Transform)->Scale<<std::endl;
							std::cout<<"Enter new scale  =  ";
							std::cin>>GrabbedObject->has(Transform)->Scale.x>>GrabbedObject->has(Transform)->Scale.y;
							GrabbedObject->has(Body)->BodyScale=Vec2(GrabbedObject->has(Transform)->Scale.x/2,GrabbedObject->has(Transform)->Scale.y/2);
							static_cast<ShapeAAB*>(GrabbedObject->has(Body)->BodyShape)->Extents=GrabbedObject->has(Body)->BodyScale;
							GrabbedObject->has(DebugGeometry)->ModifyDebugShape("edit", Vec3(GrabbedObject->has(Body)->BodyScale.x*2,GrabbedObject->has(Body)->BodyScale.y*2,0) , Vec4(1,0,0,1));
						}
					}else if( key->character == 'm' )
					{
						if(GrabbedObject!=NULL)
						{
							std::cout<<"curerent position  = "<<GrabbedObject->has(Transform)->Position<<std::endl;
							std::cout<<"Enter new Position  =  ";
							std::cin>>GrabbedObject->has(Transform)->Position.x>>GrabbedObject->has(Transform)->Position.y;
							GrabbedObject->has(Body)->Position.x=GrabbedObject->has(Transform)->Position.x;
							GrabbedObject->has(Body)->Position.y=GrabbedObject->has(Transform)->Position.y;
						}
					}
					break;
				}
			case Mid::CharacterKeyRelease:
				{
				}
			case Mid::VirtualKey:
				{
					MessageVirtualKey * key = (MessageVirtualKey*)m;
					if(key->character == VK_F3)	// mode change
					{
						m_enabled=!m_enabled;
						if(!m_enabled)
						{
							if(GrabbedObject!=NULL)
								GrabbedObject->PopLastComponent();
							GrabbedObject=NULL;
						}
						SystemPause p(false);
						CORE->BroadcastMessage(&p);

					}else if(key->character == VK_DELETE)	// delete a object
					{
						if(GrabbedObject!=NULL)
						{
							GOC* temp=GrabbedObject;
							GrabbedObject=NULL;
							temp->Destroy();
						}
					}else if(key->character == VK_UP)	
					{
						if(GrabbedObject!=NULL)
						{
							GrabbedObject->has(Transform)->Position.y+=1;
							GrabbedObject->has(Body)->Position.y+=1;
						}
					}else if(key->character == VK_DOWN)	
					{
						if(GrabbedObject!=NULL)
						{
							GrabbedObject->has(Transform)->Position.y-=1;
							GrabbedObject->has(Body)->Position.y-=1;
						}
					}else if(key->character == VK_RIGHT)	
					{
						if(GrabbedObject!=NULL)
						{
							GrabbedObject->has(Transform)->Position.x+=1;
							GrabbedObject->has(Body)->Position.x+=1;
						}
					}else if(key->character == VK_LEFT)
					{
						if(GrabbedObject!=NULL)
						{
							GrabbedObject->has(Transform)->Position.x-=1;
							GrabbedObject->has(Body)->Position.x-=1;
						}
					}
					break;
				}
			case Mid::MouseMove:
				{
					//The mouse has moved, update the mouse's world position
					MouseMove * mouseMove = (MouseMove*)m;
					GRAPHICS->ScreenToWorldSpace(&LOGIC->WorldMousePosition,&mouseMove->MousePosition);
					break;
				}
			case Mid::MouseWheel :
				{
					//zoom in/out
					MouseWheel * mouseWheel = (MouseWheel*)m;
					if (mouseWheel->Delta>0)
						LOGIC->CAMERA->has(Transform)->Position.z+=5.0f;
					else if (mouseWheel->Delta<0)
						LOGIC->CAMERA->has(Transform)->Position.z-=5.0f;
					break;
				}
			case Mid::RMouseButton:
				{
					RMouseButton * rmouse = (RMouseButton*)m;
					GRAPHICS->ScreenToWorldSpace(&LOGIC->WorldMousePosition,&rmouse->MousePosition);
					if(rmouse->ButtonIsPressed)
					{
						m_EditScale=true;
						ScaleVec=LOGIC->WorldMousePosition;
					}
					else
					{
						m_EditScale=false;
					}
					break;
				}
			case Mid::LMouseButton:
				{
					LMouseButton * lmouse = (LMouseButton*)m;
					GRAPHICS->ScreenToWorldSpace(&LOGIC->WorldMousePosition,&lmouse->MousePosition);
					if(lmouse->ButtonIsPressed)
					{
						if( GOC* object = PHYSICS->TestPoint( LOGIC->WorldMousePosition ) )
						{
							m_bodymove=true;
							if(!IsCtrlHeld())
							{
								BodyMouseDiff=object->has(Body)->Position-LOGIC->WorldMousePosition;
								if(GrabbedObject!=NULL)
								{
									if(object!=GrabbedObject)
									{
										//pop the last component
										GrabbedObject->PopLastComponent();
										DebugGeometry *debug=new DebugGeometry();
										debug->AddDebugShape(DEBUG_SHAPE_RECTANGLE, Vec3(object->has(Body)->BodyScale.x*2,object->has(Body)->BodyScale.y*2,0) , Vec4(1,0,0,1),"edit");
										object->AddComponentAndInitialize(CT_DebugGeometry,debug);
										GrabbedObject=object;
									}
								}else
								{
									DebugGeometry *debug=new DebugGeometry();
									debug->AddDebugShape(DEBUG_SHAPE_RECTANGLE, Vec3(object->has(Body)->BodyScale.x*2,object->has(Body)->BodyScale.y*2,0) , Vec4(1,0,0,1),"edit");
									object->AddComponentAndInitialize(CT_DebugGeometry,debug);
									GrabbedObject=object;
								}
							}else
							{
								if(GrabbedObject!=NULL)
										GrabbedObject->PopLastComponent();
								GrabbedObject=LevelClass->CreateObjectAt(object->archId.c_str(),LOGIC->WorldMousePosition);
								DebugGeometry *debug=new DebugGeometry();
								debug->AddDebugShape(DEBUG_SHAPE_RECTANGLE, Vec3(GrabbedObject->has(Body)->BodyScale.x*2,GrabbedObject->has(Body)->BodyScale.y*2,0) , Vec4(1,0,0,1),"edit");
								GrabbedObject->AddComponentAndInitialize(CT_DebugGeometry,debug);
							}
						}
						else
						{
							m_screenMove=true;
							BodyMouseDiff=LOGIC->WorldMousePosition;
						}
					}
					else
					{
							m_bodymove=false;
							m_screenMove=false;
					}
					break;
				}
			}
		}
		else{
			if (m->MessageId==Mid::VirtualKey)
			{
				MessageVirtualKey * key = (MessageVirtualKey*)m;
				if(key->character == VK_F4)	// edit mode
					{
						SystemPause p(true);
						CORE->BroadcastMessage(&p);
						m_enabled=true;
					}
			}
		}
	}
}