#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};

matrix mat_world;
int flag;
float3 color;

//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VSInputType
{
	float3 v0 : POSITION0;
	float3 v1 : POSITION1;
	float3 v2 : POSITION2;
	uint type : VFLAG;
};

struct GSInputType
{
	float3 v0 : POSITION0;
	float3 v1 : POSITION1;
	float3 v2 : POSITION2;
	uint type : VFLAG;
};

struct PSInputType
{
	float4 position : SV_POSITION;
	float3 view_position : POSITION;
	float bloom : TEXCOORD;
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
	vout.v0 = vin.v0;
	vout.v1 = vin.v1;
	vout.v2 = vin.v2;
	vout.type = vin.type;
	return vout;
}
//--------------------------------------------------------------------------------------
// GEOMETRY SHADER
//--------------------------------------------------------------------------------------
[maxvertexcount(2)]
void GS( point GSInputType gin[1], inout LineStream<PSInputType> gout )
{
	if ( gin[0].type < LASER_BEAM ) return;
	PSInputType v;
	
	gin[0].v2.xy *= gin[0].v0.z;
	v.bloom = gin[0].v2.x;
	
	v.view_position = mul( float4(gin[0].v0.xy,0.0,1.0), mat_view ).xyz;
	v.position = mul( float4(v.view_position,1.0f), mat_proj );
	gout.Append(v);
	v.bloom = gin[0].v2.y;
	v.view_position = mul( float4(gin[0].v1.xy,0.0,1.0), mat_view ).xyz;
	v.position = mul( float4(v.view_position,1.0f), mat_proj );
	gout.Append(v);

}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
PSOutputType PS( PSInputType pin )
{ 
	PSOutputType pout;

	pout.diffuse.xyz = color;
	pout.diffuse.w = pin.bloom;
	pout.view_position.xyz = pin.view_position;
	pout.normal_flag.w = 128;
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
