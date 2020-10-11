#define FXAA_PC			1
#define FXAA_HLSL_4		1
#define FXAA_QUALITY__PRESET 12
#define FXAA_GREEN_AS_LUMA 0

#include "Fxaa3_11.fxh"

cbuffer GSPerFrameBuffer : register(b0)
{
	matrix mat_ortho;
};

Texture2D color : register(t0);
SamplerState sstate : register(s0);
//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VSInputType
{
	float2 screen : FLOAT0;
};

struct GSInputType
{
	float2 screen : POSITION;
};

struct PSInputType
{
	float4 position : SV_POSITION;
	float2 viewport : POSITION;
	float2 tex : TEXCOORD;
};

//--------------------------------------------------------------------------------------
// VERTEX SHADER
//--------------------------------------------------------------------------------------
GSInputType VS( VSInputType vin )
{
	GSInputType vout;
	vout.screen = vin.screen;
	return vout;
}
//--------------------------------------------------------------------------------------
// GEOMETRY SHADER
//--------------------------------------------------------------------------------------
inline float4 Transform( float3 position )
{
	return mul( float4( position, 1.0f ), mat_ortho );
}
[maxvertexcount(4)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType> gout )
{
	PSInputType v;

	v.viewport = 1.0 / gin[0].screen;
	v.position = Transform( float3( 0.0, 0.0, 0.0 ) );
	v.tex = float2( 0.0, 1.0 );
	gout.Append(v);
	
	v.position = Transform( float3( 0.0, 1.0, 0.0 ) );
	v.tex = float2( 0.0, 0.0 );
	gout.Append(v);

	v.position = Transform( float3( 1.0, 0.0, 0.0 ) );
	v.tex = float2( 1.0, 1.0 );
	gout.Append(v);
	
	v.position = Transform( float3( 1.0, 1.0, 0.0 ) );
	v.tex = float2( 1.0, 0.0 );
	gout.Append(v);
}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
float4 PS( PSInputType pin ) : SV_Target
{ 
	FxaaTex t = {sstate, color};
	//return float4( pin.tex.yyy, 1.0 );
	//return color.Sample( sstate, pin.tex );
	return float4(FxaaPixelShader( pin.tex, 
							float4(0,0,0,0),
							t,
							t,
							t,
							pin.viewport,
							float4(0,0,0,0),
							float4(0,0,0,0),
							float4(0,0,0,0),
							0.75,
							0.125,
							0.0625,
							8.0,
							0.125,
							0.05,
							0 ).xyz, 
					1.0);
}
