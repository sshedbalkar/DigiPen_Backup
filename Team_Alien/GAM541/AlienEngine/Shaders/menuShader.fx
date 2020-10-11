#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};

Texture2D button_texture;
//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VSInputType
{
	float3 position : POSITION0;
	float3 extra : POSITION1;
	float2 tex : TEXCOORD0;
	uint type : VFLAG;
};

struct GSInputType
{
	float3 position : POSITION0;
	float3 extra : POSITION1;
	float2 tex : TEXCOORD0;
	uint type : VFLAG;
};

struct PSInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR;
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
	vout.position = vin.position;
	vout.extra = vin.extra;
	vout.tex = vin.tex;
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
#define T	0.1667
inline void AppendOctagon( PSInputType v, 
						   float3 center, 
						   float2 size, 
						   float2 indent, 
						   bool texed,
						   inout TriangleStream<PSInputType> stream )
{
	float2 tcenter = v.tex;
	v.tex = 0.0;
	float3 position = center;
	position.xy -= size * 0.5;
	position.y += indent.y;
	v.position = ProjectionTransform( position );
	if ( texed )
	{
		v.tex.x = tcenter.x - T;
	}
	stream.Append(v);

	position.y += size.y;
	position.y -= indent.y * 2.0;
	v.position = ProjectionTransform( position );
	stream.Append(v);

	position.x += indent.x;
	position.y -= size.y;
	position.y += indent.y;
	v.position = ProjectionTransform( position );
	if ( texed )
	{
		v.tex = tcenter;
		v.tex.x -= T;
		v.tex.y += 0.5;
	}
	stream.Append(v);

	position.y += size.y;
	v.position = ProjectionTransform( position );
	if ( texed )
		v.tex.y -= 1.0;
	stream.Append(v);

	position.x += size.x;
	position.x -= indent.x * 2.0;
	position.y -= size.y;
	v.position = ProjectionTransform( position );
	if ( texed )
	{
		v.tex.x += 2 * T;
		v.tex.y += 1.0;
	}
	stream.Append(v);

	position.y += size.y;
	v.position = ProjectionTransform( position );
	if ( texed )
		v.tex.y -= 1.0;
	stream.Append(v);

	position.x += indent.x;
	position.y -= size.y;
	position.y += indent.y;
	v.position = ProjectionTransform( position );
	if ( texed ) 
		v.tex.x = tcenter.x + T;
	stream.Append(v);

	position.y += size.y;
	position.y -= indent.y * 2.0;
	v.position = ProjectionTransform( position );
	stream.Append(v);

	stream.RestartStrip();
}
[maxvertexcount(8)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType> gout )
{
	PSInputType v;

	if ( gin[0].type < 2 )//button
	{
		float3 position = ViewingTransform( float3(gin[0].position.x + gin[0].position.z, gin[0].position.y,0.0) );
		if ( gin[0].type == 0 ) 
			v.color = float4( 0.9, 0.45, 0.8, 0.0 );
		else v.color = float4( 0.9, 0.5, 0.85, 1.0 );
		v.tex = gin[0].tex;
		AppendOctagon( v, position, gin[0].extra.xy, float2( 5.0,5.0 ), true, gout );
	}
	else if ( gin[0].type == 2 )
	{
		float3 position = ViewingTransform( float3( gin[0].position.xy, 0.0 ) );
		v.color = float4( 0.9, 0.5, 0.85, 1.0 );
		v.tex = gin[0].tex;
		AppendOctagon( v, position, gin[0].extra.zz, gin[0].extra.zz * 0.25, false, gout );
	}
}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
PSOutputType PS( PSInputType pin )
{ 
	PSOutputType pout;

	pout.diffuse = pin.color;
	float4 mask = button_texture.SampleLevel( sample_point_wrap, pin.tex, 0.0 );
	pout.diffuse.xyz *= 1.0 - mask.a;
	mask.xyz *= mask.a;
	pout.diffuse.xyz += mask.xyz;
	pout.normal_flag.w = 96;
	pout.view_position.z = 1.0;
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
