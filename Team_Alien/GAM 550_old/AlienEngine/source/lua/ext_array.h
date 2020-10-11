#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

extern int newarray( lua_State* L )
{
	int n = luaL_checkint( L, 1 );
	size_t size = n * sizeof(float);
	lua_newuserdata( L, size );

	luaL_getmetatable( L, "LuaBook.array" );
	lua_setmetatable( L, -2 );

	return 1;
}

extern float* luaL_checkarray( lua_State* L, int index )
{
	void* arr = luaL_checkudata( L, index, "LuaBook.array" );
	return (float*)arr;
}

extern int setarray( lua_State* L )
{
	float* arr = luaL_checkarray( L, 1 );
	int n = lua_gettop(L);
	//if ( n > arr->size + 1 ) n = arr->size;
	int i;
	for ( i = 2; i <= n; ++i )
	{
		arr[i - 2] = (float)luaL_checknumber( L, i );
	}
	return 0;
}

extern int getarray( lua_State* L )
{
	float* arr = luaL_checkarray( L, 1 );
	int n = luaL_checkint( L, 2 );
	lua_pushnumber( L, arr[n] );

	printf( "%0.3f %0.3f %0.3f %0.3f\n", *arr, *(arr+1), *(arr+2), *(arr+3) ); arr += 4;
	printf( "%0.3f %0.3f %0.3f %0.3f\n", *arr, *(arr+1), *(arr+2), *(arr+3) ); arr += 4;
	printf( "%0.3f %0.3f %0.3f %0.3f\n", *arr, *(arr+1), *(arr+2), *(arr+3) ); arr += 4;
	printf( "%0.3f %0.3f %0.3f %0.3f\n\n", *arr, *(arr+1), *(arr+2), *(arr+3) ); arr += 4;
	return 1;
}

extern int luaopen_array( lua_State* L )
{
	struct luaL_reg arraylib_f [] = 
	{
		{ "New", newarray },
		{ NULL, NULL }
	};
	struct luaL_reg arraylib_m [] =
	{
		{ "Set", setarray },
		{ "Get", getarray },
		{ NULL, NULL }
	};
	luaL_newmetatable( L, "LuaBook.array" );
	lua_pushstring( L, "__index" );
	lua_pushvalue( L, -2 );
	lua_settable( L, -3 );
	luaL_openlib( L, NULL, arraylib_m, 0 );
	luaL_openlib( L, "array", arraylib_f, 0 );
	return 0;
}

