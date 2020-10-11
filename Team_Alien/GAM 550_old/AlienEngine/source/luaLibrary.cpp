#include "Precompiled.h"
#include "lua.h"
#include "luaLibrary.h"

namespace Framework
{

std::vector< std::pair<int (*) ( lua_State*, void* ), void*> > LuaLibrary::library_function_list;

void LuaLibrary::Add( LuaLibraryFunc func, void* userdata )
{
	library_function_list.push_back( std::pair<LuaLibraryFunc, void*>( func, userdata ) );
}
void LuaLibrary::Open( lua_State* L )
{
	for ( unsigned int i = 0; i < library_function_list.size(); ++i )
	{
		library_function_list[i].first( L, library_function_list[i].second );
	}
}


}