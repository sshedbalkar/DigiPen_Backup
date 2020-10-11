#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};

matrix mat_world;
Texture2D normal_map;
Texture2D diffuse;

//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VSInputType
{
	float3 position : POSITION0;
	float3 tip : POSITION1;
};

struct GSInputType
{
	float3 position : POSITION0;
	float3 tip : POSITION1;
};

struct PSInputType
{
	float4 position : SV_POSITION;
	float3 view_position : VIEW;
	float intensity : TEXCOORD0;
	float2 tex : TEXCOORD1;
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
GSInputType VS( VSInputType vin )
{
	GSInputType vout;
	vout.position = vin.position;
	vout.tip = vin.tip;
	return vout;
}
//--------------------------------------------------------------------------------------
// GEOMETRY SHADER
//--------------------------------------------------------------------------------------
inline float4 ProjectionTransform( float3 position )
{
	return mul( float4( position, 1.0 ), mat_proj );
}
inline float3 WorldTransform( float3 position )
{
	return mul( float4( position,1.0 ), mat_world ).xyz;
}
inline float3 ViewingTransform( float3 position )
{
	return mul( float4( position, 1.0 ), mat_view ).xyz;
}
#define SCALE 0.04
[maxvertexcount(6)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType> gout )
{
	PSInputType v;
	float3 position;
	float2 size = float2( gin[0].position.y, gin[0].position.z );
	float i;
	float y;
//	half_size *= 0.5f;

	position = float3( gin[0].position.x, 0.0, 0.0 );
	position.x -= size.x * 0.5;
	v.view_position = WorldTransform( position );
	i = ( v.view_position.z ) / 600.0;
	y = v.view_position.y;
	v.intensity = i;
	v.tex = v.view_position.xy;
	v.tex *= SCALE;
	v.view_position = ViewingTransform( v.view_position );
	v.position = ProjectionTransform( v.view_position );
	gout.Append(v);
	
	position.x += size.x;
	v.view_position = WorldTransform( position );
	v.tex = v.view_position.xy;
	v.tex *= SCALE;
	v.view_position = ViewingTransform( v.view_position );
	v.position = ProjectionTransform( v.view_position );
	gout.Append(v);
		
	position.y += size.y * gin[0].tip.x;
	position.x -= size.x;
	v.view_position = WorldTransform( position );
	v.tex = v.view_position.xy;
	v.tex *= SCALE;
	v.intensity = i * ( 400.0 - ( v.view_position.y - y ) ) * 0.0025;
	v.view_position = ViewingTransform( v.view_position );
	v.position = ProjectionTransform( v.view_position );
	gout.Append(v);
	
	position.x += size.x;
	v.view_position = WorldTransform( position );
	v.tex = v.view_position.xy;
	v.tex *= SCALE;
	v.view_position = ViewingTransform( v.view_position );
	v.position = ProjectionTransform( v.view_position );
	gout.Append(v);

	position.y = size.y;
	position.x -= size.x;
	position.x += gin[0].tip.y * size.x;
	v.view_position = WorldTransform( position );
	v.tex = v.view_position.xy;
	v.tex *= SCALE;
	v.intensity = i * ( 400.0 - ( v.view_position.y - y ) ) * 0.0025;
	v.view_position = ViewingTransform( v.view_position );
	v.position = ProjectionTransform( v.view_position );
	gout.Append(v);
	
	position.x += gin[0].tip.z * size.x;
	v.view_position = WorldTransform( position );
	v.tex = v.view_position.xy;
	v.tex *= SCALE;
	v.view_position = ViewingTransform( v.view_position );
	v.position = ProjectionTransform( v.view_position );
	gout.Append(v);
}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
PSOutputType PS( PSInputType pin )
{ 
	PSOutputType pout;
	
	float3 normal = float3( 0,0,-1 );
	if ( pin.view_position.z < 250.0 )
	{
		float3 bump = normal_map.SampleLevel( sample_linear_wrap, pin.tex * 2.0, 0 ).xyz;
		bump.xy -= 0.5f;
		bump *= 2.0f;
		normal += bump.x * float3( 1, 0, 0 );
		normal += bump.y * float3(0,-1,0);
		normal += bump.z * float3(0,0,-1);
	}
	pout.normal_flag.xyz = normal;



	pout.normal_flag.w = 37;
//	pout.diffuse.xyz *= bump;
//	pout.diffuse = normal_map.SampleLevel( sample_linear_wrap, pin.tex, 0 );
//	pout.diffuse.xyz = pin.tex.x.xxx;
	pout.diffuse.w = 0.0;
	pout.view_position.xyz = pin.view_position;
	pout.view_position.w = 30.0;

	float4 color = diffuse.SampleLevel( sample_linear_wrap, pin.tex, 0.0 );
	pout.diffuse.xyz = color.xyz * pin.intensity;

	return pout;
}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 Shader
{
	pass pass0
	{
		SetBlendState( noblend, float4(0,0,0,0), 0xffffffff );
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( CompileShader( gs_4_0, GS() ) );		
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }

}
