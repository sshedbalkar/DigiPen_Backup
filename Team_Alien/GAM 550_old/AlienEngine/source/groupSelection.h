#pragma once

namespace Framework
{

class GameObjectComposition;
class Camera;

class GroupSelection
{
public:
	typedef std::list<GameObjectComposition*> GOCListType;

private:
	GOCListType object_list;

public:
	GroupSelection();
	~GroupSelection();

	void Select( float x0, float y0, float x1, float y1, Camera* camera );
	void Select( float x, float y, Camera* camera );

	void Clear();
	GOCListType::iterator Begin();
	GOCListType::iterator End();
};

}