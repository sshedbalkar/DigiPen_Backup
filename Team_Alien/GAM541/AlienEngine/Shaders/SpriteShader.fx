#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};

Texture2D color;

uint flag;
float3 position;
float2 size;
float rotation;

//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VSInputType
{
//	float3 position : POSITION0;
//	float3 size_rotation : POSITION1;
};

struct GSInputType
{
//	float3 position : POSITION0;
//	float3 size_rotation : POSITION1;
};

struct PSInputType
{
	float4 position : SV_POSITION;
	float3 view_position : VIEW;
	float2 tex : TEXCOORD;
};

struct PSOutputType
{
	float4 diffuse : SV_Target0;
	float4 normal_flag: SV_Target1;
	float4 view_position : SV_Target2;
};

//--------------------------------------------------------------------------------------
// VERTEX SHADER
//--------------------------------------------------------------------------------------
GSInputType VS( void )
{
	GSInputType vout;
	return vout;
//	vout.position = vin.position;
//	vout.size_rotation = vin.size_rotation;
//	return vout;
}
//--------------------------------------------------------------------------------------
// GEOMETRY SHADER
//--------------------------------------------------------------------------------------
float4 ProjectionTransform( float3 position )
{
	return mul( float4( position, 1.0f ), mat_proj );
}
float3 ViewingTransform( float3 position )
{
	return mul( float4(position,1.0f), mat_view ).xyz;
}
[maxvertexcount(4)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType> gout )
{
	PSInputType v;

	float2 pos;
//	float2 size = gin[0].size_rotation.xy;
	float c = cos( rotation );
	float s = sin( rotation );
	float2x2 mat_rot = {{c,-s},{s,c}};

	pos = float2( -size.x, -size.y );
	pos = mul( pos, mat_rot );
	v.view_position = ViewingTransform( float3( pos + position.xy, position.z ) );
	v.position = ProjectionTransform( v.view_position );
	v.tex = float2( 0.0f, 1.0f );
	gout.Append(v);
	
	pos = float2( size.x, -size.y );
	pos = mul( pos, mat_rot );
	v.view_position = ViewingTransform( float3( pos + position.xy, position.z ) );
	v.position = ProjectionTransform( v.view_position );
	v.tex = float2( 1.0f, 1.0f );
	gout.Append(v);
	
	pos = float2( -size.x, size.y );
	pos = mul( pos, mat_rot );
	v.view_position = ViewingTransform( float3( pos + position.xy, position.z ) );
	v.position = ProjectionTransform( v.view_position );
	v.tex = float2( 0.0f, 0.0f );
	gout.Append(v);
	
	pos = float2( size.x, size.y );
	pos = mul( pos, mat_rot );
	v.view_position = ViewingTransform( float3( pos + position.xy, position.z ) );
	v.position = ProjectionTransform( v.view_position );
	v.tex = float2( 1.0f, 0.0f );
	gout.Append(v);
}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
PSOutputType PS( PSInputType pin )
{ 
	PSOutputType pout;

	pout.diffuse = color.SampleLevel( sample_linear_wrap, pin.tex, 0 );
	pout.view_position.xyz = pin.view_position;
	pout.view_position.w = 1.0;
	pout.normal_flag.xyz = float3(0,0,-1);
	pout.normal_flag.w = (float)flag;
	return pout;
}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 SpriteShader
{
	pass pass0
	{
		SetBlendState( alpha_blend, float4(0,0,0,0), 0xffffffff );
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( CompileShader( gs_4_0, GS() ) );		
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }

}
