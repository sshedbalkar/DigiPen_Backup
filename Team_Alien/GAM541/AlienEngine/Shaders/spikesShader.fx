#include "states.fxh"
#define TILE_HEIGHT		0.05f
#define SHININESS		30.0f

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};

matrix mat_world;

//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VSInputType
{
	float3 position : POSITION;
};

struct GSInputType
{
	float3 position : POSITION;
};

struct PSInputType
{
	float4 position : SV_POSITION;
	float3 view_position : POSITION;
	float3 normal : NORMAL;
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
	return vout;
}
//--------------------------------------------------------------------------------------
// GEOMETRY SHADER
//--------------------------------------------------------------------------------------
inline float3 WorldViewingTransform( float3 position )
{
	float4 ret = mul( float4( position, 1.0f ), mat_world );
	ret = mul( ret, mat_view );
	return ret.xyz;
}
inline float4 ProjectionTransform( float3 position )
{
	return mul( float4(position,1.0f), mat_proj );
}
[maxvertexcount(6)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType> gout )
{
	PSInputType v;
	v.normal = mul( float3( 0.0, 0.0, -1.0 ), (float3x3)mat_view );

	float3 position = float3( gin[0].position.x, 0.5, 0.0 );
	position.x -= gin[0].position.y * 0.5;
	v.view_position = WorldViewingTransform( position );
	v.position = ProjectionTransform( v.view_position );
	v.view_position.z -= 1.0;
	gout.Append(v);

	position.x += gin[0].position.y;
	v.view_position = WorldViewingTransform( position );
	v.position = ProjectionTransform( v.view_position );
	v.view_position.z -= 1.0;
	gout.Append(v);
	
	position.x = gin[0].position.x;
	v.view_position = WorldViewingTransform( position );
	v.view_position.y += gin[0].position.z;
	v.position = ProjectionTransform( v.view_position );
	v.view_position.z -= 1.0;
	gout.Append(v);
	gout.RestartStrip();
	
	//position = float3( gin[0].position.x + gin[0].position.y * 0.7, 0.4, 0.0 );
	//position.x -= gin[0].position.y * 0.5;
	//v.view_position = WorldViewingTransform( position );
	//v.view_position.z += 0.5;
	//v.position = ProjectionTransform( v.view_position );
	//gout.Append(v);
//
	//position.x += gin[0].position.y;
	//v.view_position = WorldViewingTransform( position );
	//v.view_position.z += 0.5;
	//v.position = ProjectionTransform( v.view_position );
	//gout.Append(v);
	//
	//position.x -= gin[0].position.y * 0.5;
	//v.view_position = WorldViewingTransform( position );
	//v.view_position.y += gin[0].position.z;
	//v.view_position.z += 0.5;
	//v.position = ProjectionTransform( v.view_position );
	//gout.Append(v);
}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
PSOutputType PS( PSInputType pin )
{ 
	PSOutputType pout;

	pout.normal_flag.xyz = pin.normal;
	pout.normal_flag.w = 37;
	pout.diffuse = float4( 0.7, 0.0, 0.0, 0.0 );
	pout.view_position.xyz = pin.view_position;
	pout.view_position.w = 20.0;
	return pout;
}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 SpikesShader
{
	pass pass0
	{
		SetBlendState( noblend, float4(0,0,0,0), 0xffffffff );
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( CompileShader( gs_4_0, GS() ) );		
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }

}
