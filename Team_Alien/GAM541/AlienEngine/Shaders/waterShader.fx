#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};

float t;
float ripple_height;
float ripple_x;
float ripple_initial_t;
Texture2D diffuse_map;

//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VSInputType
{
	float3 position : POSITION0;
};

struct GSInputType
{
	float3 position : POSITION0;
};

struct PSInputType
{
	float4 position : SV_POSITION;
	float3 view_position : VIEW;
	float2 tex : TEXCOORD0;
	float intensity : TEXCOORD1;
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
inline float4 ProjectionTransform( float3 position )
{
	float4 projected = mul( float4( position, 1.0 ), mat_proj );
	projected.xy /= projected.w;
	position.z -= 1.0;
	projected.zw = mul( float4( position, 1.0 ), mat_proj ).zw;
	projected.xy *= projected.w;

	return projected;
}
inline float3 ViewingTransform( float3 position )
{
	return mul( float4( position, 1.0 ), mat_view ).xyz;
}
#define TEX_SCALE 0.5
#define PIECE_WIDTH 0.11
#define TEX_MOVING_SPEED	0.15

#define WAVE_LENGTH		1.0
#define RIPPLE_LENGTH	5.0
#define SPEED	-1.0
#define RIPPLE_SPEED	10.0
#define WAVE_HEIGHT		0.12
[maxvertexcount(4)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType> gout )
{
	PSInputType v;
	float3 position;
	float2 height;
	float offset_tex = TEX_MOVING_SPEED * t;
	float ripple_range = t + 0.01;
	ripple_range -= ripple_initial_t;
	ripple_range *= RIPPLE_SPEED;
	float h = 0.0;
	float ripple_t;
	if ( ripple_height > 0.01 )
	{
		h = gin[0].position.x - ripple_x;
		ripple_t = h > 0 ? -RIPPLE_SPEED : RIPPLE_SPEED;
		ripple_t *= t - ripple_initial_t - 1.57;
		h = abs(h);
		h = -h;
		h += ripple_range;
		h /= ripple_range;
		if ( h > 0.0 )
			h *= h;
		h *= ripple_height;
	}

	position = float3( gin[0].position.xy, 0.0 );
	position.x -= PIECE_WIDTH * 0.5;
	position.y -= gin[0].position.z * 0.5;
	height.x = WAVE_HEIGHT * sin( position.x * WAVE_LENGTH + t * SPEED );
	if ( h > 0.01 )
	{
		height.x += h * cos( ( position.x - ripple_x ) * RIPPLE_LENGTH + ripple_t );
	}
	v.tex = position.xy;
	v.tex *= TEX_SCALE;
	v.tex.y -= offset_tex;
	v.view_position = ViewingTransform( position );
	v.position = ProjectionTransform( v.view_position );
	v.intensity = 0.9;
	gout.Append(v);
	
	position.x += PIECE_WIDTH;
	height.y = WAVE_HEIGHT * sin( position.x * WAVE_LENGTH + t * SPEED );
	if ( h > 0.01 )
	{
		height.y += h * cos( ( position.x - ripple_x ) * RIPPLE_LENGTH + ripple_t );
	}
	v.tex = position.xy;
	v.tex *= TEX_SCALE;
	v.tex.y -= offset_tex;
	v.view_position = ViewingTransform( position );
	v.position = ProjectionTransform( v.view_position );
	gout.Append(v);


	position.y += gin[0].position.z;
	position.y += height.x;
	position.x -= PIECE_WIDTH;
	v.tex = position.xy;
	v.tex *= TEX_SCALE;
	v.tex.y -= offset_tex;
	v.view_position = ViewingTransform( position );
	v.position = ProjectionTransform( v.view_position );
	v.intensity = 0.6;
	gout.Append(v);
	
	position.y -= height.x;
	position.y += height.y;
	position.x += PIECE_WIDTH;
	v.tex = position.xy;
	v.tex *= TEX_SCALE;
	v.tex.y -= offset_tex;
	v.view_position = ViewingTransform( position );
	v.position = ProjectionTransform( v.view_position );
	gout.Append(v);
}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
PSOutputType PS( PSInputType pin )
{ 
	PSOutputType pout;
	
	pout.normal_flag.xyz = float3( 0,0,-1 );
	pout.normal_flag.w = 37;
	pout.diffuse = diffuse_map.SampleLevel( sample_linear_wrap, pin.tex, 0 );
	pout.diffuse *= pin.intensity;
	pout.diffuse.w = 0.2;
	pout.view_position.xyz = pin.view_position;
	pout.view_position.w = 20.0;
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
