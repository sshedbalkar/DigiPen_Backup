#pragma once
//
#include "Component.h"
#include <deque>
//
namespace Framework
{

class ScriptComponent: public GameComponent
{
public:
										ScriptComponent();
	virtual								~ScriptComponent();
	//
	virtual void						Serialize( ISerializer& stream );
	virtual void						Initialize();
	virtual void						Free();
	int									TestFn1( int n );
	//
private:
	typedef std::deque< std::string >	ScriptList;
	ScriptList							m_scripts;
};

}
