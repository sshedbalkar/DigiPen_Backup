#pragma once
#include "Precompiled.h"
#include "AntTweakBar.h"
#include "System.h"
#include "Component.h"
#include "Vector3.h"
#include "Torus.h"
#include "Axis.h"
#include "EditorFunctionality.h"

namespace Framework
{

class TweakBarManager;
class LevelEditor : public ISystem
{
	friend class Axis;
	friend class EditorFunctionality ;
private:
	enum EditorLevelState
	{
		select,
		move,
		rotate,
		scale,
		CreateObjectetype,
		numberOfStates
	};
	Axis * axisObject;
	EditorFunctionality * editorFunc;
	EditorLevelState state;
	Vector3 position,direction;
	TweakBarManager * twbmanager;
	Vec3 center;
	Vector3 clickedpoint,currentpoint,topPoint,downPoint;
	Vector3 clickedPosition,unClickedPosition,MousePosition,prevMousePosition,screenMousePosition,prevScreenMousePosition;
	GOC * m_gocToCopy;//may change to vector later for many objects
	bool isContolHeld,drawbox;
	bool leftclicked;
	void LeftClick();
	void LeftUnClick();
	typedef std::vector<GOC*> GameObjects;
	typedef std::vector<GOC*>::iterator gameObjectIterator;
	GameObjects selectedObjects;
public:
	Vector3 CenterDir,CenterPos;
	LevelEditor();
	~LevelEditor();
	virtual void Update( float dt );
	virtual void Initialize();
	virtual void Free();
	virtual void SendMessage( Message* m );
	virtual void Unload();
	virtual std::string GetName() { return "LevelEditor"; }
	void enterEditMode(){inEditor=true;}
	void Draw(const D3DXMATRIX& worldMatrix,const D3DXMATRIX& viewMatrix,const D3DXMATRIX& projectionMatrix);
	bool inEditor;
	int maxSelectDistance;
};

extern LevelEditor* EDITOR;
}
