#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

extern int StringGetParam( lua_State* L )
{
	const char* stream = luaL_checkstring( L, 1 );
	const char* format = luaL_checkstring( L, 2 );
	int i;
	float f;
	char s[21];
	int pcount = 0;

	while ( *format != 0 )
	{
		while( *stream == ' ' ) ++stream;
		switch( *format )
		{
		case 'i':
			if ( sscanf_s( stream, "%d", &i ) )
				lua_pushnumber( L, i );
			else 
			{
				lua_pushnumber( L, 0 );
				printf("#warning: failed reading an integar from command line.\n");
			}
			break;
		case 'f':
			if ( sscanf_s( stream, "%f", &f ) )
				lua_pushnumber( L, f );
			else
			{
				lua_pushnumber( L, 0.0f );
				printf("#warning: failed reading a floating number from command line.\n");
			}
			break;
		case 's':
			if ( sscanf_s( stream, "%s", s, sizeof(s) ) )
				lua_pushstring( L, s );
			else
			{
				lua_pushstring( L, "" );
				printf("#warning: failed reading a string from command line.\n");
			}
			break;
		default:
			break;
		}
		
		while( *stream != ' ' && *stream != 0 ) ++stream;
		++format;
		++pcount;
	}
	return pcount;
}

extern int StringPathGetFilename( lua_State* L )
{
	const char* path = luaL_checkstring( L, 1 );
	int i;
	for ( i = strlen( path ) - 1; i >= 0; --i )
	{
		if ( path[i] == '\\' ) break;
	}
	lua_pushstring( L, path + i + 1 );
	return 1;
}

extern int luaopen_mstring( lua_State* L )
{
	struct luaL_reg stringlib_f [] = 
	{
		{ "Parameter", StringGetParam },
		{ "PathGetFilename", StringPathGetFilename },
		{ NULL, NULL }
	};

	luaL_openlib( L, "mstring", stringlib_f, 0 );
	return 0;
}

