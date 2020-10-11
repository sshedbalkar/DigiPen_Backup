#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};

matrix mat_world;
int flag;
float4 color;

//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VSInputType
{
	float3 position0 : POSITION0;
	float3 position1 : POSITION1;
	uint type : VFLAG;
};

struct GSInputType
{
	float3 position0 : POSITION0;
	float3 position1 : POSITION1;
	uint type : VFLAG;
};

struct PSInputType
{
	float4 position : SV_POSITION;
	float bloom : BLOOM;
	//float3 view_position : POSITION;
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
	vout.position0 = vin.position0;
	vout.position1 = vin.position1;
	vout.type = vin.type;
	return vout;
}
//--------------------------------------------------------------------------------------
inline float3 ViewingTransform( float3 position )
{
	return mul( float4(position,1.0), mat_view ).xyz;
}
inline float4 ProjectionTransform( float3 position )
{
	return mul( float4(position,1.0), mat_proj );
}
//--------------------------------------------------------------------------------------
// GEOMETRY SHADER
//--------------------------------------------------------------------------------------
[maxvertexcount(2)]
void GS( point GSInputType gin[1], inout LineStream<PSInputType> gout )
{
	if ( gin[0].type == 1 ) return;
	PSInputType v;
	v.bloom = gin[0].position0.z;
	float3 position;
	position = ViewingTransform( float3(gin[0].position0.xy, 0.0) );
	v.position = ProjectionTransform( position );
	gout.Append(v);

	position = ViewingTransform( float3(gin[0].position1.xy, 0.0) );
	v.position = ProjectionTransform( position );
	gout.Append(v);

}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
PSOutputType PS( PSInputType pin )
{ 
	PSOutputType pout;

	pout.diffuse = float4( 1.0,0.9,0.9,pin.bloom );
	pout.normal_flag.w = 64;
	pout.view_position.z = 0.1;
	return pout;
}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 ParticleShader
{
	pass pass0
	{
		SetBlendState( noblend, float4(0,0,0,0), 0xffffffff );
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( CompileShader( gs_4_0, GS() ) );		
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }

}
