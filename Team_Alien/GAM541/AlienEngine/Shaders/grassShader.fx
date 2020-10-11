#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};
matrix mat_world;
//
//SamplerState sample_bilinear
//{
    //Filter = MIN_MAG_MIP_POINT;
    //AddressU = Wrap;
    //AddressV = Wrap;
//};

//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VSInputType
{
	float3 position : POSITION;
	float3 extra : NORMAL;			// x = half-width, y = height, z = bend;
	float4 color : COLOR;
};

struct GSInputType
{
	float3 position : POSITION;
	float3 extra : NORMAL;			// x = half-width, y = height, z = bend;
	float4 color : COLOR;
};

struct PSInputType
{
	float3 view_position : VIEW;
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR;
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
	vout.color = vin.color;
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

void CalculatePoint( float t, float3 P, float3 Q, float3 R, out float3 pos )
{
	float a0 = 1.0f - t;
	float a1 = a0 * t * 2.0f;
	float a2 = t * t;
	a0 *= a0;
	pos = a0 * P + a1 * Q + a2 * R;
}
[maxvertexcount(10)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType> gout )
{
	PSInputType v;
	float bend = gin[0].extra.z;
//	bend *= 1.05f;
	float3 P = gin[0].position;
	float3 O = float3(0,0,0);
	P = WorldViewingTransform( P );
	O = WorldViewingTransform( O );
	float z = P.z;
	float2 zw = ProjectionTransform( P ).zw;
	P.z = O.z;
	float3 Q = float3( P.x, P.y + gin[0].extra.y * 0.5f, O.z );
	float3 R = float3( P.x + gin[0].extra.y * sin(bend), P.y + gin[0].extra.y * cos(bend), O.z );

	int lod = 4;
	float sq_lod = (float)lod  * (float)lod;

	float4 color = gin[0].color;

	for ( int i = lod; i >= 0; --i )
	{
		float3 mid;
		CalculatePoint( (float)i / (float)lod, P, Q, R, mid );
		float width = gin[0].extra.x * ( sq_lod - pow( (float)i, 2 ) ) / sq_lod;

		v.view_position = float3( mid.x - width, mid.yz );
		v.position = ProjectionTransform( v.view_position );
		v.view_position.z = z;
		v.position.xy /= v.position.w;
		v.position.xy *= zw.y;
		v.position.zw = zw;
		v.color = color;
		v.tex = float2(0.0f, (float)(lod-i) / (float)lod );
		gout.Append(v);
	
		v.view_position = float3( mid.x + width, mid.yz );
		v.position = ProjectionTransform( v.view_position );
		v.view_position.z = z;
		v.position.xy /= v.position.w;
		v.position.xy *= zw.y;
		v.position.zw = zw;
		v.color = color;
		v.tex = float2(1.0f, (float)(lod-i) / (float)lod );
		gout.Append(v);
	}
}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
PSOutputType PS( PSInputType pin )
{ 
	PSOutputType pout;

	pout.normal_flag.xyz = float3( 0, 0, -1 );
	pout.normal_flag.w = 33;
	pout.diffuse.xyz = pin.color.xyz;
	pout.diffuse.w = 0.0;
	pout.view_position.xyz = pin.view_position;

	return pout;
}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 GrassShader
{
	pass pass0
	{
		SetBlendState( noblend, float4(0,0,0,0), 0xffffffff );
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( CompileShader( gs_4_0, GS() ) );		
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }

}
