#include <string.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

extern int PointerAppendChunk( lua_State* L )
{
	char** ptr = (char**)luaL_checkudata( L, 1, "LuaBook.chunk" );
	void** chunk = (void**)luaL_checkudata( L, 2, "LuaBook.chunk" );
	int size = luaL_checkint( L, 3 );

	memcpy( *ptr, *chunk, size );
	(*ptr) += size;

	return 0;
}

extern int PointerAppendArray( lua_State* L )
{
	char** ptr = (char**)luaL_checkudata( L, 1, "LuaBook.chunk" );
	float* chunk = (float*)luaL_checkudata( L, 2, "LuaBook.array" );
	int size = luaL_checkint( L, 3 );
	size *= sizeof(float);
	memcpy( *ptr, chunk, size );
	(*ptr) += size;

	return 0;
}

extern int PointerGetFloat( lua_State* L )
{
	void** ptr = (void**)luaL_checkudata( L, 1, "LuaBook.chunk" );
	float* data = (float*)*ptr;
	int num = luaL_checkint( L, 2 );
	int i;
	for ( i = 0; i < num; ++i )
	{
		lua_pushnumber( L, *data );
		++data;
	}
	return num;
}

extern int PointerGetIntegar( lua_State* L )
{
	void** ptr = (void**)luaL_checkudata( L, 1, "LuaBook.chunk" );
	int* data = (int*)*ptr;
	int num = luaL_checkint( L, 2 );
	int i;
	for ( i = 0; i < num; ++i )
	{
		lua_pushnumber( L, *data );
		++data;
	}
	return num;
}

extern int PointerSetFloat( lua_State* L )
{
	void** ptr = (void**)luaL_checkudata( L, 1, "LuaBook.chunk" );
	float* data = (float*)*ptr;
	int num = lua_gettop( L );
	int i;
	float temp;
	for ( i = 1; i < num; ++i )
	{
		temp = (float)luaL_checknumber( L, i + 1 );
		*data = temp;
		++data;
	}
	return 0;
}

extern int PointerSetIntegar( lua_State* L )
{
	void** ptr = (void**)luaL_checkudata( L, 1, "LuaBook.chunk" );
	int* data = (int*)*ptr;
	int num = lua_gettop( L );
	int i;
	int temp;
	for ( i = 1; i < num; ++i )
	{
		temp = luaL_checkint( L, i + 1 );
		*data = temp;
		++data;
	}
	return 0;
}

extern int PointerSetString( lua_State* L )
{
	void** ptr = (void**)luaL_checkudata( L, 1, "LuaBook.chunk" );
	const char* str = luaL_checkstring( L, 2 );
	int maxl = luaL_checkint( L, 3 );
	int len = strlen( str );
	if ( len > maxl )
	{
		memcpy( *ptr, str, maxl );
	}
	else
		memcpy( *ptr, str, len + 1 );
	return 0;
}

extern int PointerOffset( lua_State* L )
{
	char** ptr = (char**)luaL_checkudata( L, 1, "LuaBook.chunk" );
	char** pdata = (char**)luaL_checkudata( L, 2, "LuaBook.chunk" );
	int num = luaL_checkint( L, 3 );

	*pdata = *ptr;
	*pdata += num;

	return 0;
}

extern int luaopen_chunk( lua_State* L )
{
	struct luaL_reg chunklib_m [] = 
	{
		{"AppendArray", PointerAppendArray },
		{"AppendChunk", PointerAppendChunk },
		{"GetF", PointerGetFloat },
		{"GetI", PointerGetIntegar },
		{"SetF", PointerSetFloat },
		{"SetI", PointerSetIntegar },
		{"SetS", PointerSetString },
		{"Offset", PointerOffset },

		{NULL, NULL}
	};

	luaL_newmetatable( L, "LuaBook.chunk" );
	lua_pushstring( L, "__index" );
	lua_pushvalue( L, -2 );
	lua_settable( L, -3 );
	luaL_openlib( L, NULL, chunklib_m, 0 );
	return 0;
}