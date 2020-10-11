#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};

float2 viewport;
Texture2D bloom_source;

//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct GSInputType
{
	float2 size : SIZE;
};

struct PSInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

float sample_size;
//--------------------------------------------------------------------------------------
// VERTEX SHADER
//--------------------------------------------------------------------------------------
GSInputType VS( void )
{
	GSInputType vout;
	vout.size = 1.0 / viewport;
	return vout;
}
//--------------------------------------------------------------------------------------
// GEOMETRY SHADER
//--------------------------------------------------------------------------------------
float4 Transform( float3 position )
{
	float4 pos = float4( position, 1.0f );
	pos = mul( pos, mat_view );
	pos = mul( pos, mat_proj );
	return pos;
}
[maxvertexcount(4)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType> gout )
{
	PSInputType v;

	v.position = Transform( float3( 0.0, 0.0, 0.0 ) );
	v.tex = float2( 0.0f,1.0f ) ;
	gout.Append(v);
	
	v.position = Transform( float3( gin[0].size.x, 0.0, 0.0 ) );
	v.tex = float2( 1.0f,1.0f ) ;
	gout.Append(v);
	
	v.position = Transform( float3( 0.0, gin[0].size.y, 0.0f ) );
	v.tex = float2( 0.0f, 0.0f );
	gout.Append(v);
	
	v.position = Transform( float3( gin[0].size.x, gin[0].size.y, 0.0f ) );
	v.tex = float2( 1.0f, 0.0f ) ;
	gout.Append(v);
}

//const static float3x3 kernel = { 0.01, 0.08, 0.01, 
								 //0.08, 0.64, 0.08
								 //0.01, 0.08, 0.01 };
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
float4 PS( PSInputType pin ) : SV_Target
{
	float4 result = 0;
	float4 colorM = bloom_source.Sample( sample_linear_clamp, pin.tex );
	float2 offset;
	float STEP = 4.0 / sample_size;

//	offset = float2( -STEP, -STEP );
//	result += 0.02 * bloom_source.Sample( sample_linear_clamp, pin.tex + offset * viewport );
	offset = float2( 0, -STEP );
	result += 0.09 * bloom_source.Sample( sample_linear_clamp, pin.tex + offset * viewport );
//	offset = float2( STEP, -STEP );
//	result += 0.02 * bloom_source.Sample( sample_linear_clamp, pin.tex + offset * viewport );
	offset = float2( -STEP, 0 );
	result += 0.09 * bloom_source.Sample( sample_linear_clamp, pin.tex + offset * viewport );
	result += 0.40 * colorM;
	offset = float2( STEP, 0 );
	result += 0.09 * bloom_source.Sample( sample_linear_clamp, pin.tex + offset * viewport );
//	offset = float2( -STEP, STEP );
//	result += 0.02 * bloom_source.Sample( sample_linear_clamp, pin.tex + offset * viewport );
	offset = float2( 0, STEP );
	result += 0.09 * bloom_source.Sample( sample_linear_clamp, pin.tex + offset * viewport );
//	offset = float2( STEP, STEP );
//	result += 0.02 * bloom_source.Sample( sample_linear_clamp, pin.tex + offset * viewport );

	result.w = 0.0;
	return result;
}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 DeferredShader
{
    pass pass0
    {
		SetBlendState( noblend, float4(0,0,0,0),0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );		
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }    
}
