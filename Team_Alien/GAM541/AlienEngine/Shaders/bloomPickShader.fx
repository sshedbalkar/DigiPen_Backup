#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};

float2 viewport;
Texture2D diffuse;
Texture2D view_position;

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
	
	v.position = Transform( float3( 0.0, gin[0].size.y, 0.0 ) );
	v.tex = float2( 0.0f, 0.0f );
	gout.Append(v);
	
	v.position = Transform( float3( gin[0].size.x, gin[0].size.y, 0.0f ) );
	v.tex = float2( 1.0f, 0.0f ) ;
	gout.Append(v);
}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
float4 PS( PSInputType pin ) : SV_Target
{
	float4 colorM = diffuse.Sample( sample_linear_clamp, pin.tex );
	if ( colorM.w > 0.01f )
		return float4( colorM.xyz * ( colorM.w ), 0.0 );

	//float thickness = 2.0f;
	//float depthM = view_position.SampleLevel( sample_linear_clamp, pin.tex, 0.0 ).z;
	//float depthN = view_position.SampleLevel( sample_linear_clamp, pin.tex + float2( 0.0,-thickness ) * viewport, 0.0 ).z;
	//float depthW = view_position.SampleLevel( sample_linear_clamp, pin.tex + float2(-thickness, 0.0 ) * viewport, 0.0 ).z;
	//float depthS = view_position.SampleLevel( sample_linear_clamp, pin.tex + float2( 0.0, thickness ) * viewport, 0.0 ).z;
	//float depthE = view_position.SampleLevel( sample_linear_clamp, pin.tex + float2( thickness, 0.0 ) * viewport, 0.0 ).z;
		//
	//float max_depth = max( depthN, max( depthW, max( depthS, depthE ) ) );
	//
	//float contrast_depth = max_depth / depthM;
//
	//if ( contrast_depth > 1.01f ) return float4( colorM.xyz * 0.5f, 0.0 );
	return 0.0;
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
