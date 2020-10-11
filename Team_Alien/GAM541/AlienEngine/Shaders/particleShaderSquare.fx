#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};

int flag;
float4 color;
//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VSInputType
{
	float3 position : POSITION0;
	float3 velocity : POSITION1;
	float3 v2_timer : POSITION2;
	uint type : VFLAG;
};

struct GSInputType
{
	float3 position : POSITION0;
	float3 velocity : POSITION1;
	float3 v2_timer : POSITION2;
	uint type : VFLAG;
};

struct PSInputType
{
	float4 position : SV_POSITION;
	float3 view_position : POSITION;
	float3 tex_bloom : TEXCOORD0;
	uint type : VFLAG;
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
	vout.velocity = vin.velocity;
	vout.v2_timer = vin.v2_timer;
	vout.type = vin.type;
	return vout;
}
//--------------------------------------------------------------------------------------
inline float2 MapTexcoordToQuad( float2 tex, float2 offset, float2 size )
{
}
//--------------------------------------------------------------------------------------
// GEOMETRY SHADER
//--------------------------------------------------------------------------------------
[maxvertexcount(4)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType> gout )
{
	if ( ( gin[0].type & PT_LAUNCHER) == PT_LAUNCHER ) return;
	matrix mat_translation = {{1,0,0,0},
							  {0,1,0,0},
							  {0,0,1,0},
							  {gin[0].position.x,gin[0].position.y,gin[0].position.z,1}};
	matrix mat_worldview = mul( mat_translation, mat_view );
//	mat_worldview = mul( mat_worldview, mat_view );

	float c = cos(gin[0].v2_timer.x);
	float s = sin(gin[0].v2_timer.x);
	matrix mat_rot = {{c,-s,0,0},{s,c,0,0},{0,0,1,0},{0,0,0,1}};
	mat_worldview = mul( mat_rot, mat_worldview );

	PSInputType v;
	float3 position;
	float radius = gin[0].v2_timer.y;
	
	v.type = gin[0].type;
	v.tex_bloom.z = gin[0].velocity.z;

	position = float3( -radius.xx, 0.0f );
	v.view_position = mul( float4(position,1.0f), mat_worldview ).xyz;
	v.position = mul( float4(v.view_position,1.0f), mat_proj );
	v.tex_bloom.xy = float2( 0.0f, 1.0f );
	gout.Append(v);

	position.x = radius;
	v.view_position = mul( float4(position,1.0f), mat_worldview ).xyz;
	v.position = mul( float4(v.view_position,1.0f), mat_proj );
	v.tex_bloom.xy = float2( 1.0f, 1.0f );
	gout.Append(v);
	
	position.x = -radius;
	position.y = radius;
	v.view_position = mul( float4(position,1.0f), mat_worldview ).xyz;
	v.position = mul( float4(v.view_position,1.0f), mat_proj );
	v.tex_bloom.xy = float2( 0.0f, 0.0f );
	gout.Append(v);
	
	position.x = radius;
	v.view_position = mul( float4(position,1.0f), mat_worldview ).xyz;
	v.position = mul( float4(v.view_position,1.0f), mat_proj );
	v.tex_bloom.xy = float2( 1.0f, 0.0f );
	gout.Append(v);
}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
PSOutputType PS( PSInputType pin )
{ 
	PSOutputType pout;

	pout.diffuse = color;
	//if ( pout.diffuse.w > 1.0 ) pout.diffuse.w = 2.0 - pout.diffuse.w;
	pout.view_position.xyz = pin.view_position;
//	pout.view_position.w = 0;
	pout.normal_flag.w = (float)flag;
	return pout;
}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 ParticleShader
{
	pass pass0
	{
//		SetBlendState( noblend, float4(0,0,0,0), 0xffffffff );
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( CompileShader( gs_4_0, GS() ) );		
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }

}
