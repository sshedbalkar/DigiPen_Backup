#pragma once

namespace Framework
{

class GameObjectComposition;

class LogicHierarchy
{
	typedef std::vector<LogicHierarchy*> HierarchyListType;

private:
	HierarchyListType hierarchy_list;
	GameObjectComposition* object;

};

}