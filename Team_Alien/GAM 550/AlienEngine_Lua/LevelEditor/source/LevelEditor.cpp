#include "Precompiled.h"
#include "LevelEditor.h"
#include "Physics.h"
#include "Graphics.h"
#include "WindowsSystem.h"
#include "Factory.h"
#include "Core.h"
#include "Utility.h"
#include <ctime>
#include "particleEmitter.h"
#include "LoadLevelClass.h"
#include "Level_1.h"
#include "debugGeometry.h"
#include "GSM.h"
#include "TweakBarManager.h"
#include "VMath.h"
#include "SaveLevelClass.h"
#include "ModelComponent.h"
#include "Model.h"

namespace Framework
{
	LevelEditor* EDITOR=NULL;

	LevelEditor::LevelEditor()
	{
		EDITOR=this;
		state=select;
		inEditor=false;
		leftclicked=false;
		twbmanager = new TweakBarManager;
		m_gocToCopy=NULL;
		maxSelectDistance = 20;
		axisObject= new Axis;
		editorFunc = new EditorFunctionality;
		
	}

	LevelEditor::~LevelEditor()
	{
	}

	
	void LevelEditor::Initialize()
	{
		twbmanager->InitializeTweakBar();
		axisObject->Initialize();
	}
	
	void LevelEditor::Free()
	{
		TwTerminate();
	}
	
	void LevelEditor::SendMessage( Message* m )
	{
		if(!inEditor) return;
		switch(m->MessageId){
		case Mid::CharacterKey:
			{
				MessageCharacterKey * key = (MessageCharacterKey*)m;
				if(key->character==VK_ESCAPE)
				{
					SaveClass->SaveLevel("harsha.xml"); 
					inEditor=false;
					state=select;
					m_gocToCopy = 0;
					twbmanager->HideAllBars();
					for(unsigned i = 0 ; i<selectedObjects.size() ; ++i){
						ModelComponent* pmc = selectedObjects[i]->has(ModelComponent);
						if(pmc) pmc->ChangeTextureBack();
					}
					GRAPHICS->TurnOffDebugCamera();
					for(ObjectLinkList<RigidBody>::iterator it=PHYSICS->RigidBodies.begin();it!=PHYSICS->RigidBodies.end();++it)
					{
						it->editorInitialize();
						it->setAwake(true);
					}
					for(ObjectLinkList<RigidBody>::iterator it=PHYSICS->LevelData.begin();it!=PHYSICS->LevelData.end();++it)
					{
						it->editorInitialize();
					}
				}
				else if(key->character=='e')
				{
					if(state==CreateObjectetype)
					{
						twbmanager->BuildFromArchetype(twbmanager);
					}
				}
				else if(key->character=='1')
				{
					state=select;
				}else if(key->character=='2')
				{
					state=move;
				}else if(key->character=='3')
				{
					state=rotate;
				}
				else if(key->character=='4')
				{
					state=scale;
				}
				else if(key->character=='5')
				{
					state=CreateObjectetype;
					twbmanager->ShowCreateObjectDetails();
				}
				else if(key->character=='t')
				{
					for(int i=0;i<selectedObjects.size();++i){
						//selectedObjects[i]->has(ModelComponent)->ModelName="cube2";
						selectedObjects[i]->has(ModelComponent)->SwapModel();}
				}
			
				break;
			}
		case Mid::VirtualKey:
			{
				// Cast to the derived message type
				MessageVirtualKey * key = (MessageVirtualKey*)m;
				if(key->character == VK_F5)	// save game
					SaveClass->SaveLevel("test.xml");
				else if(key->character==0x43)
				{
					if (selectedObjects.size()==1)
						m_gocToCopy=selectedObjects[0];


				}
				else if(key->character==0x56 )
				{
					if (selectedObjects.size()==1)
						editorFunc->CopySelectedObjects();
				}
				else if(key->character==0x41 )
				{
					GameObjectIdMapType mapOfGOCS = FACTORY->GetAllObjects() ;
					GameObjectIdMapType::iterator p;
					selectedObjects.clear();
					state=move;
					for(p = mapOfGOCS.begin(); p != mapOfGOCS.end(); p++){
						selectedObjects.push_back(p->second);
						ModelComponent* pmc = p->second->has(ModelComponent);
						if(pmc) pmc->ChangeTextureDebug();
					}
				}
				else if(key->character==VK_DELETE )
				{
					if (selectedObjects.size()!=0)
						editorFunc->DeleteSelectedObjects();
				}
				break;
			}
		case Mid::LMouseButton:
			{
				LMouseButton * lmouse = (LMouseButton*)m;
				if(lmouse->ButtonIsPressed)
				{
					clickedPosition=Vector3(lmouse->MousePosition.x,lmouse->MousePosition.y,0.0);
					clickedpoint = clickedPosition;
					GRAPHICS->screenPosition(clickedpoint);
					LeftClick();
				}
				else
				{
					unClickedPosition=Vector3(lmouse->MousePosition.x,lmouse->MousePosition.y,0.0);
					LeftUnClick();
					leftclicked=false;
				}
				break;
			}
		case Mid::MouseMove:
			{

				//The mouse has moved, update the mouse's world position
				MouseMove * mouseMove = (MouseMove*)m;
				MousePosition=Vector3(mouseMove->MousePosition.x,mouseMove->MousePosition.y,0.0);
				position.x=mouseMove->MousePosition.x;
				position.y=mouseMove->MousePosition.y;
				position.z=0;
				screenMousePosition=MousePosition;
				currentpoint = MousePosition;
				GRAPHICS->screenPosition(currentpoint);
				topPoint=Vector3(MousePosition.x,clickedPosition.y,0);
				downPoint=Vector3(clickedPosition.x,MousePosition.y,0);
				if(leftclicked)
				{
					if(abs(topPoint.x-clickedPosition.x)>10 || abs(downPoint.y-clickedPosition.y)>10)
						drawbox=true;
					else
						drawbox=false;
				}else 
					drawbox=false;
				GRAPHICS->screenPosition(topPoint);
				GRAPHICS->screenPosition(downPoint);
				direction=Vector3(0,0,0);
				CenterDir=direction;
				CenterPos=Vector3(GRAPHICS->screen_width/2,GRAPHICS->screen_height/2,0);
				GRAPHICS->ScreenToWorldSpaceRay(CenterPos,CenterDir);
				GRAPHICS->ScreenToWorldSpaceRay(position,direction);
				GRAPHICS->ScreenToWorldSpace(MousePosition,center.z);
				break;
			}
		}
	}
	
	void LevelEditor::Unload()
	{
	}

	void LevelEditor::LeftClick()
	{
		switch(state)
		{
		case select:
			{
				leftclicked=true;
				break;
			}
		case move:
			{
				//movement code
				leftclicked=true;
				break;
			}
		case rotate:
			{
				//rotate code
				leftclicked=true;
				break;
			}
		case scale:
			{
				//scale code
				break;
			}
		}

	}

	void LevelEditor::LeftUnClick()
	{
		switch(state)
		{
		case select:
			{
				//select code
				if(abs(unClickedPosition.x-clickedPosition.x)<10 && abs(unClickedPosition.y-clickedPosition.y)<10)
				{					
					if(selectedObjects.size()!=0)
					{
						state=move;
						twbmanager->SetSelectedGOC(selectedObjects[0]);
						std::cout<<selectedObjects[0]->archId<<std::endl;
					}else
					twbmanager->SetSelectedGOC(NULL);
				}else
				{
					//multiple selection code
					for(unsigned i = 0 ; i<selectedObjects.size() ; ++i){
						ModelComponent* pmc = selectedObjects[i]->has(ModelComponent);
						if(pmc) pmc->ChangeTextureBack();
					}
					selectedObjects.clear();
					editorFunc->SelectObjects();
					if(selectedObjects.size()!=0)
						state=move;
					if(selectedObjects.size()==1)
						twbmanager->SetSelectedGOC(selectedObjects[0]);
					else
						twbmanager->SetSelectedGOC(NULL);
				}
				break;
			}
		case move:
			{
				//movement code
				break;
			}
		case rotate:
			{
				//rotate code
				break;
			}
		case scale:
			{
				//scale code
				break;
			}
		}
	}


	void LevelEditor::Draw(const D3DXMATRIX& worldMatrix,const D3DXMATRIX& viewMatrix,const D3DXMATRIX& projectionMatrix)
	{
		if(!inEditor) return;
		center=Vec3(0,0,0);
		if(selectedObjects.size()!=0)
		{
			for(int i=0;i<selectedObjects.size();++i)
				center+=selectedObjects[i]->has(Transform)->Position;
			center/=selectedObjects.size();
		}
		switch(state)
		{
		case select:
			{
				//select code
				//code for edge detection i guess
				if(leftclicked && drawbox)
				{
					Vec4 color(1,0,0,1);
					//std::cout<<"a";
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3((float)clickedpoint.x,(float)clickedpoint.y,(float)clickedpoint.z), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( Vec3((float)downPoint.x, (float)downPoint.y, (float)downPoint.z), color) );

					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3( (float)downPoint.x, (float)downPoint.y, (float)downPoint.z), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( Vec3( (float)currentpoint.x, (float)currentpoint.y, (float)currentpoint.z), color) );

					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3( (float)clickedpoint.x, (float)clickedpoint.y, (float)clickedpoint.z), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( Vec3( (float)topPoint.x, (float)topPoint.y, (float)topPoint.z), color) );

					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3( (float)topPoint.x, (float)topPoint.y, (float)topPoint.z), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( Vec3( (float)currentpoint.x, (float)currentpoint.y, (float)currentpoint.z), color) );

				}
				break;
			}
		case move:
			{
				
				//movement code

				//xaxis
				Vec4 color(1,0,0,1);
				if(axisObject->selectedAxis==axisObject->AxisSelection::x) 
					color=Vec4(1,1,1,1);
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(center, color) );
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( Vec3(center.x+1.2f,center.y,center.z), color) );
				real a=R_PI/4,angle=0;
				for(int i=0;i<8;++i)
				{
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3( (float)center.x+1.2f, (float)center.y+0.1*cos(angle),center.z+0.1*sin(angle)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3( (float)center.x+1.2f, (float)center.y+0.1*cos(angle+a),center.z+0.1*sin(angle+a)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3( (float)center.x+1.2f, (float)center.y+0.1*cos(angle),center.z+0.1*sin(angle)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3( (float)center.x+1.5f, (float)center.y, (float)center.z), color));
					angle+=a;
				}

				//yaxis
				color=Vec4(0,1,0,1);
				if(axisObject->selectedAxis == axisObject->AxisSelection::y) 
					color=Vec4(1,1,1,1);
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(center, color));
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( Vec3(center.x,center.y+1.2f,center.z), color));
				a=R_PI/4;
				angle=0;
				for(int i=0;i<8;++i)
				{
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+0.1*cos(angle),center.y+1.2f,center.z+0.1*sin(angle)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+0.1*cos(angle+a),center.y+1.2,center.z+0.1*sin(angle+a)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+0.1*cos(angle),center.y+1.2,center.z+0.1*sin(angle)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x,center.y+1.5,center.z), color));
					angle+=a;
				}

				//zaxis
				color=Vec4(0,0,1,1);
				if(axisObject->selectedAxis==axisObject->AxisSelection::z) 
					color=Vec4(1,1,1,1);
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(center, color) );
				GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex( Vec3(center.x,center.y,center.z+1.5f), color));
				a=R_PI/4;
				angle=0;
				for(int i=0;i<8;++i)
				{
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+0.1*cos(angle),center.y+0.1*sin(angle),center.z+1.2), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+0.1*cos(angle+a),center.y+0.1*sin(angle+a),center.z+1.2), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+0.1*cos(angle),center.y+0.1*sin(angle),center.z+1.2), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x,center.y,center.z+1.5), color));
					angle+=a;
				}
				break;
			}
		case rotate:
			{
				//rotate code
				//xaxis
				Vec4 color(0,0,1,1);
				if(axisObject->selectedRotationAxis==axisObject->AxisSelection::x) 
					color=Vec4(1,1,1,1);
				real a=R_PI/16,angle=0;
				for(int i=0;i<32;++i)
				{
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x,center.y+2*cos(angle),center.z+2*sin(angle)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x,center.y+2*cos(angle+a),center.z+2*sin(angle+a)), color) );
					angle+=a;
				}

				//yaxis
				color=Vec4(1,0,0,1);
				if(axisObject->selectedRotationAxis==axisObject->AxisSelection::y) 
					color=Vec4(1,1,1,1);
				a=R_PI/16;
				angle=0;
				for(int i=0;i<32;++i)
				{
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+2*cos(angle),center.y,center.z+2*sin(angle)), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+2*cos(angle+a),center.y,center.z+2*sin(angle+a)), color) );
					angle+=a;
				}

				//zaxis
				color=Vec4(0,1,0,1);
				if(axisObject->selectedRotationAxis==axisObject->AxisSelection::z) 
					color=Vec4(1,1,1,1);
				a=R_PI/16;
				angle=0;
				for(int i=0;i<32;++i)
				{
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+2*cos(angle),center.y+2*sin(angle),center.z), color) );
					GRAPHICS->lineDrawer.drawLineList.push_back( LineVertex(Vec3(center.x+2*cos(angle+a),center.y+2*sin(angle+a),center.z), color) );
					angle+=a;
				}
				break;
			}
		case scale:
			{
				//scale code
				break;
			}
		}
	}


	void LevelEditor::Update(float dt)
	{
		
		if(!inEditor) return;//NOTHING should be above this code
		//std::cout<<FACTORY->GetAllObjects().size()<<std::endl;

		if (IsCtrlHeld()){
			isContolHeld=true;
		}
		else{
			isContolHeld=false;
		}

		switch(state)
		{
		case select:
			{
				//select code
				for(unsigned i = 0 ; i<selectedObjects.size() ; ++i){
					ModelComponent* pmc = selectedObjects[i]->has(ModelComponent);
					if(pmc) pmc->ChangeTextureBack();
				}
				selectedObjects.clear();
				axisObject->Update(dt);
				GOC*a=editorFunc-> GetGOCFromIntersection(position,direction);
				if(a){
					selectedObjects.push_back(a);
					ModelComponent* amc = a->has(ModelComponent);
					if(amc) amc->ChangeTextureDebug();
				}
				break;
			}
		case move:
			{
				//movement code
				if(selectedObjects.size()==0) 
				{
					state=select;
					break;
				}
				axisObject->Update(dt);
				if(leftclicked)
					editorFunc->UpdatePosition();
				else
					axisObject->CheckAxes();
				break;
			}
		case rotate:
			{
				//rotate code
				if(selectedObjects.size()==0)
				{
					state=select;
					break;
				}
				if(leftclicked)
					editorFunc->UpdateRotation();
				else
					axisObject->CheckRotationaxes();
				break;
			}
		case scale:
			{
				//scale code
				if(selectedObjects.size()==0)
				{
					state=select;
					break;
				}
				break;
			}
		}
		center=Vec3(0,0,0);
		if(selectedObjects.size()!=0)
		{
			for(int i=0;i<selectedObjects.size();++i)
				center+=selectedObjects[i]->has(Transform)->Position;
			center/=selectedObjects.size();
		}
		
		twbmanager->Update();
		prevMousePosition=MousePosition;
		prevScreenMousePosition=screenMousePosition;
	}


	

}
