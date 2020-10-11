#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};

Texture2D font_texture;

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

BlendState AlphaBlendingState
{
    BlendEnable[0] = TRUE;
    DestBlend = INV_SRC_ALPHA;
};

float2 world_position;
//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VSInputType
{
	float3 position0 : POSITION;
	float3 position1 : NORMAL;
	float4 color : COLOR;
	float2 tex0 : TEXCOORD0;
	float2 tex1 : TEXCOORD1;
};

struct GSInputType
{
	float3 position0 : POSITION;
	float3 position1 : NORMAL;
	float4 color : COLOR;
	float2 tex0 : TEXCOORD0;
	float2 tex1 : TEXCOORD1;
};

struct PSInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 tex : TEXCOORD0;
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
	vout.color = vin.color;
	vout.tex0 = vin.tex0;
	vout.tex1 = vin.tex1;
	return vout;
}
//--------------------------------------------------------------------------------------
// GEOMETRY SHADER
//--------------------------------------------------------------------------------------
float4 Transform( float3 position )
{
	float4 pos = float4( position, 1.0f );
	pos.xy += world_position;
	pos = mul( pos, mat_view );
	pos = mul( pos, mat_proj );
	return pos;
}
#define GLOW	2.0
[maxvertexcount(8)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType> gout )
{
	PSInputType v;
	
	v.color = gin[0].color;
	v.color.a = 0.5;
	

	v.position = Transform( gin[0].position0 + float3( -GLOW,-GLOW,0.0 ) );
	v.tex = float2( gin[0].tex0.x - gin[0].tex1.x, gin[0].tex0.y + gin[0].tex1.y ) ;
	gout.Append(v);
	
	v.position = Transform( float3( gin[0].position1.x + GLOW, gin[0].position0.y-GLOW, 0.0 ) );
	v.tex = float2( gin[0].tex0.x + gin[0].tex1.x, gin[0].tex0.y + gin[0].tex1.y ) ;
	gout.Append(v);
	
	v.position = Transform( float3( gin[0].position0.x-GLOW.x, gin[0].position1.y+GLOW, 0.0 ) );
	v.tex = float2( gin[0].tex0.x - gin[0].tex1.x, gin[0].tex0.y - gin[0].tex1.y ) ;
	gout.Append(v);
	
	v.position = Transform( gin[0].position1 + float3( GLOW,GLOW,0.0 ) );
	v.tex = float2( gin[0].tex0.x + gin[0].tex1.x, gin[0].tex0.y - gin[0].tex1.y ) ;
	gout.Append(v);

	v.color.a = 1.0;
	v.position = Transform( gin[0].position0 );
	v.tex = float2( gin[0].tex0.x - gin[0].tex1.x, gin[0].tex0.y + gin[0].tex1.y ) ;
	gout.Append(v);
	
	v.position = Transform( float3( gin[0].position1.x, gin[0].position0.yz ) );
	v.tex = float2( gin[0].tex0.x + gin[0].tex1.x, gin[0].tex0.y + gin[0].tex1.y ) ;
	gout.Append(v);
	
	v.position = Transform( float3( gin[0].position0.x, gin[0].position1.yz ) );
	v.tex = float2( gin[0].tex0.x - gin[0].tex1.x, gin[0].tex0.y - gin[0].tex1.y ) ;
	gout.Append(v);
	
	v.position = Transform( gin[0].position1 );
	v.tex = float2( gin[0].tex0.x + gin[0].tex1.x, gin[0].tex0.y - gin[0].tex1.y ) ;
	gout.Append(v);
}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
PSOutputType PS( PSInputType pin )
{ 
	PSOutputType pout;
	pout.diffuse = font_texture.Sample(SampleType, pin.tex );
	pout.diffuse = float4( pin.color.xyz, pout.diffuse.x * pin.color.a );
	pout.normal_flag.w = 64;
	return pout;

}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 FontShader
{
    pass pass0
    {
        SetBlendState(alpha_blend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );		
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }    
}
