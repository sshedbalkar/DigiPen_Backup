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
	float intensity : TEXCOORD;
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
inline float3 ViewingTransform( float3 position )
{
	return mul( float4(position,1.0), mat_view ).xyz;
}
inline float4 ProjectionTransform( float3 position )
{
	return mul( float4(position,1.0), mat_proj );
}
inline void AppendOctagon( PSInputType v, 
						   float3 center, 
						   float size,
						   inout TriangleStream<PSInputType> stream )
{
	float3 position = center;
	position.xy -= size * 0.5;
	position.y += size * 0.3;
	v.view_position = ViewingTransform( position );
	v.position = ProjectionTransform( v.view_position );
	stream.Append(v);

	position.y += size * 0.5;
	v.view_position = ViewingTransform( position );
	v.position = ProjectionTransform( v.view_position );
	stream.Append(v);

	position.x += size * 0.3;
	position.y -= size * 0.7;
	v.view_position = ViewingTransform( position );
	v.position = ProjectionTransform( v.view_position );
	stream.Append(v);

	position.y += size;
	v.view_position = ViewingTransform( position );
	v.position = ProjectionTransform( v.view_position );
	stream.Append(v);

	position.x += size.x * 0.5;
	position.y -= size;
	v.view_position = ViewingTransform( position );
	v.position = ProjectionTransform( v.view_position );
	stream.Append(v);

	position.y += size;
	v.view_position = ViewingTransform( position );
	v.position = ProjectionTransform( v.view_position );
	stream.Append(v);

	position.x += size * 0.3;
	position.y -= size * 0.7;
	v.view_position = ViewingTransform( position );
	v.position = ProjectionTransform( v.view_position );
	stream.Append(v);

	position.y += size * 0.5;
	v.view_position = ViewingTransform( position );
	v.position = ProjectionTransform( v.view_position );
	stream.Append(v);

	stream.RestartStrip();
}

[maxvertexcount(8)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType> gout )
{
	if ( gin[0].type != CLOUDS_PARTICLE ) return;
	PSInputType v;
	v.intensity = 0.3 + gin[0].v1.y * 0.4;
	float3 pos = gin[0].v0;
	pos.y += 0.2 * gin[0].v1.x;
	AppendOctagon( v, pos, gin[0].v1.x, gout );
}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
PSOutputType PS( PSInputType pin )
{ 
	PSOutputType pout;

	pout.diffuse = pin.intensity.xxxx;
	pout.diffuse.x *= 0.9;
	pout.diffuse.a = 0.25;
	pout.view_position.xyz = pin.view_position;
	pout.normal_flag.w = 0;
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
