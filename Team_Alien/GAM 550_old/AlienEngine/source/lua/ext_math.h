#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <D3DX10math.h>
#include <math.h>

extern int Vec3Normalize( lua_State* L );
extern int Vec2Dot( lua_State* L );
extern int Vec2Length( lua_State* L );
extern int Mat3SpinTilt( lua_State* L );
extern int Mat3Multiply( lua_State* L );
extern int Mat3toMat4( lua_State* L );
extern int Mat4PreTranslate( lua_State* L );
extern int Mat4PostTranslate( lua_State* L );
extern int Mat3BackTransform( lua_State* L );
extern int Mat4Multiply( lua_State* L );
extern int Mat4Inverse( lua_State* L );
extern int Mat4Transpose( lua_State* L );
extern int Mat4TransformVec( lua_State* L );
//extern int Mat4

///////////////////////////////////////////////////////////////////////////
extern int Vec2Dot( lua_State* L )
{
	double x0 = luaL_checknumber( L, 1 );
	double y0 = luaL_checknumber( L, 2 );
	double x1 = luaL_checknumber( L, 3 );
	double y1 = luaL_checknumber( L, 4 );

	double dot = x0 * x1 + y0 * y1;

	lua_pushnumber( L, dot );

	return 1;
}
///////////////////////////////////////////////////////////////////////////
extern int Vec2Length( lua_State* L )
{
	double x = luaL_checknumber( L, 1 );
	double y = luaL_checknumber( L, 2 );
	double len = x * x;
	len += y * y;
	len = sqrt(len);
	lua_pushnumber( L, len );

	return 1;
}
///////////////////////////////////////////////////////////////////////////
extern int Vec3Normalize( lua_State* L )
{
	double x = luaL_checknumber( L, 1 );
	double y = luaL_checknumber( L, 2 );
	double z = luaL_checknumber( L, 3 );
	double len = x * x;
	len += y * y;
	len += z * z;
	len = sqrt(len);
	if ( len > 0.001 )
	{
		x /= len;
		y /= len;
		z /= len;
	}
	lua_pushnumber( L, x );
	lua_pushnumber( L, y );
	lua_pushnumber( L, z );

	return 3;
}
///////////////////////////////////////////////////////////////////////////
extern int Mat3SpinTilt( lua_State* L )
{
	float* matout = (float*)luaL_checkudata( L, 1, "LuaBook.array" );
	float spin = (float)luaL_checknumber( L, 2 );
	float tilt = (float)luaL_checknumber( L, 3 );
	
	float cs = cos(-spin);
	float ss = sin(-spin);
	float ct = cos(-tilt);
	float st = sin(-tilt);
	matout[0] = cs;
	matout[1] = ss * st;
	matout[2] = ss * ct;
	matout[3] = 0.0f;
	matout[4] = ct;
	matout[5] = -st;
	matout[6] = -ss;
	matout[7] = cs * st;
	matout[8] = cs * ct;
	return 0;
}
///////////////////////////////////////////////////////////////////////////
extern int Mat3Multiply( lua_State* L )
{
	float* matout = (float*)luaL_checkudata( L, 1, "LuaBook.array" );
	float* mat0 = (float*)luaL_checkudata( L, 2, "LuaBook.array" );
	float* mat1 = (float*)luaL_checkudata( L, 3, "LuaBook.array" );

	float ret[9];
	ret[0] = mat0[0] * mat1[0]; ret[0] += mat0[1] * mat1[3]; ret[0] += mat0[2] * mat1[6];
	ret[1] = mat0[0] * mat1[1]; ret[1] += mat0[1] * mat1[4]; ret[1] += mat0[2] * mat1[7];
	ret[2] = mat0[0] * mat1[2]; ret[2] += mat0[1] * mat1[5]; ret[2] += mat0[2] * mat1[8];
	
	ret[3] = mat0[3] * mat1[0]; ret[3] += mat0[4] * mat1[3]; ret[3] += mat0[5] * mat1[6];
	ret[4] = mat0[3] * mat1[1]; ret[4] += mat0[4] * mat1[4]; ret[4] += mat0[5] * mat1[7];
	ret[5] = mat0[3] * mat1[2]; ret[5] += mat0[4] * mat1[5]; ret[5] += mat0[5] * mat1[8];
	
	ret[6] = mat0[6] * mat1[0]; ret[6] += mat0[7] * mat1[3]; ret[6] += mat0[8] * mat1[6];
	ret[7] = mat0[6] * mat1[1]; ret[7] += mat0[7] * mat1[4]; ret[7] += mat0[8] * mat1[7];
	ret[8] = mat0[6] * mat1[2]; ret[8] += mat0[7] * mat1[5]; ret[8] += mat0[8] * mat1[8];

	memcpy( matout, ret, sizeof(float) * 9 );
	return 0;
}//////////////////////////////////////////////////////////////////////////////////////////////////
extern int Mat4Multiply( lua_State* L )
{
	float* matout = (float*)luaL_checkudata( L, 1, "LuaBook.array" );
	float* mat0 = (float*)luaL_checkudata( L, 2, "LuaBook.array" );
	float* mat1 = (float*)luaL_checkudata( L, 3, "LuaBook.array" );

	D3DXMatrixMultiply( (D3DXMATRIX*)matout, (D3DXMATRIX*)mat0, (D3DXMATRIX*)mat1 );
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
extern int Mat3toMat4( lua_State* L )
{
	float* mat4out = (float*)luaL_checkudata( L, 1, "LuaBook.array" );
	float* mat3in = (float*)luaL_checkudata( L, 2, "LuaBook.array" );

	memcpy( mat4out, mat3in, sizeof(float) * 3 );
	memcpy( mat4out + 4, mat3in + 3, sizeof(float) * 3 );
	memcpy( mat4out + 8, mat3in + 6, sizeof(float) * 3 );
	memset( mat4out + 12, 0, sizeof(float) * 3 );
	mat4out[3] = mat4out[7] = mat4out[11] = 0.0f;
	mat4out[15] = 1.0f;

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
extern int Mat4PreTranslate( lua_State* L )
{
	float* mat4inout = (float*)luaL_checkudata( L, 1, "LuaBook.array" );
	float tx = (float)luaL_checknumber( L, 2 );
	float ty = (float)luaL_checknumber( L, 3 );
	float tz = (float)luaL_checknumber( L, 4 );

	mat4inout[3] += mat4inout[0] * tx; mat4inout[3] += mat4inout[1] * ty; mat4inout[3] += mat4inout[2] * tz; 
	mat4inout[7] += mat4inout[4] * tx; mat4inout[7] += mat4inout[5] * ty; mat4inout[7] += mat4inout[6] * tz; 
	mat4inout[11] += mat4inout[8] * tx; mat4inout[11] += mat4inout[9] * ty; mat4inout[11] += mat4inout[10] * tz; 

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
extern int Mat4PostTranslate( lua_State* L )
{
	float* mat4inout = (float*)luaL_checkudata( L, 1, "LuaBook.array" );
	float tx = (float)luaL_checknumber( L, 2 );
	float ty = (float)luaL_checknumber( L, 3 );
	float tz = (float)luaL_checknumber( L, 4 );

	mat4inout[3] += tx;
	mat4inout[7] += ty;
	mat4inout[11] += tz;

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
extern int Mat3BackRotate( lua_State* L )
{
	float* mat3in = (float*)luaL_checkudata( L, 1, "LuaBook.array" );
	float ix = (float)luaL_checknumber( L, 2 );
	float iy = (float)luaL_checknumber( L, 3 );
	float iz = (float)luaL_checknumber( L, 4 );
	float ox, oy, oz;
	ox = mat3in[0] * ix; ox += mat3in[3] * iy; ox += mat3in[6] * iz;
	oy = mat3in[1] * ix; oy += mat3in[4] * iy; oy += mat3in[7] * iz;
	oz = mat3in[2] * ix; oz += mat3in[5] * iy; oz += mat3in[8] * iz;
	lua_pushnumber( L, ox );
	lua_pushnumber( L, oy );
	lua_pushnumber( L, oz );

	return 3;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
extern int Mat4Transpose( lua_State* L )
{
	float* mat4inout = (float*)luaL_checkudata( L, 1, "LuaBook.array" );
	D3DXMatrixTranspose( (D3DXMATRIX*)mat4inout, (D3DXMATRIX*)mat4inout );
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
extern int Mat4Inverse( lua_State* L )
{
	float* mat4out = (float*)luaL_checkudata( L, 1, "LuaBook.array" );
	float* mat4in = (float*)luaL_checkudata( L, 2, "LuaBook.array" );
	D3DXMatrixInverse( (D3DXMATRIX*)mat4out, NULL, (D3DXMATRIX*)mat4in );
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
extern int Mat4TransformVec( lua_State* L )
{
	D3DXVECTOR4 vec, vecout;
	float* mat4in;
	vec.x = (float)luaL_checknumber( L, 1 );
	vec.y = (float)luaL_checknumber( L, 2 );
	vec.z = (float)luaL_checknumber( L, 3 );
	vec.w = 1.0f;
	mat4in = (float*)luaL_checkudata( L, 4, "LuaBook.array" );
	D3DXVec4Transform( &vecout, &vec, (D3DXMATRIX*)mat4in );
	vecout.x /= vecout.w;
	vecout.y /= vecout.w;
	vecout.x *= 0.5f;vecout.y *= 0.5f;
	vecout.x += 0.5f;vecout.y += 0.5f;

	lua_pushnumber( L, vecout.x );
	lua_pushnumber( L, vecout.y );

	return 2;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
extern int luaopen_mmath( lua_State* L )
{
	struct luaL_reg mathlib_f [] = 
	{
		{ "dot2", Vec2Dot },
		{ "len2", Vec2Length },
		{ "Vec3Normalize", Vec3Normalize },
		{ "Mat3SpinTilt", Mat3SpinTilt },
		{ "Mat3Multi", Mat3Multiply },
		{ "Mat4Multi", Mat4Multiply },
		{ "Mat3toMat4", Mat3toMat4 },
		{ "Mat4PreTranslate", Mat4PreTranslate },
		{ "Mat4PostTranslate", Mat4PostTranslate },
		{ "Mat3BackRotate", Mat3BackRotate },
		{ "Mat4Transpose", Mat4Transpose },
		{ "Mat4Inverse", Mat4Inverse },
		{ "Mat4Transform", Mat4TransformVec },
		{ NULL, NULL }
	};

	luaL_openlib( L, "mmath", mathlib_f, 0 );
	return 0;
}

