#pragma once

struct lua_State;

namespace Framework
{

class LuaLibrary
{
	typedef int (*LuaLibraryFunc) ( lua_State*, void* );

	static std::vector< std::pair<LuaLibraryFunc, void*> > library_function_list;

public:
	static void Add( LuaLibraryFunc func, void* userdata );
	static void Open( lua_State* L );
};

}