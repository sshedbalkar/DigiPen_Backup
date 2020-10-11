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
    AddressU = Wrap;
    AddressV = Wrap;
};

BlendState AlphaBlendingState
{
    BlendEnable[0] = TRUE;
    DestBlend = INV_SRC_ALPHA;
};

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
	pos = mul( pos, mat_view );
	pos = mul( pos, mat_proj );
	return pos;
}
[maxvertexcount(4)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType> gout )
{
	PSInputType v;

	v.position = Transform( gin[0].position0 );
	v.color = gin[0].color;
	v.tex = float2( gin[0].tex0.x - gin[0].tex1.x, gin[0].tex0.y + gin[0].tex1.y ) ;
	gout.Append(v);
	
	v.position = Transform( float3( gin[0].position1.x, gin[0].position0.yz ) );
	v.color = gin[0].color;
	v.tex = float2( gin[0].tex0.x + gin[0].tex1.x, gin[0].tex0.y + gin[0].tex1.y ) ;
	gout.Append(v);
	
	v.position = Transform( float3( gin[0].position0.x, gin[0].position1.yz ) );
	v.color = gin[0].color;
	v.tex = float2( gin[0].tex0.x - gin[0].tex1.x, gin[0].tex0.y - gin[0].tex1.y ) ;
	gout.Append(v);
	
	v.position = Transform( gin[0].position1 );
	v.color = gin[0].color;
	v.tex = float2( gin[0].tex0.x + gin[0].tex1.x, gin[0].tex0.y - gin[0].tex1.y ) ;
	gout.Append(v);
}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
float4 PS( PSInputType pin ) : SV_Target
{ 
	float4 pout;
	pout = font_texture.Sample(SampleType, pin.tex );
	if ( pout.r == 0.0f ) pout.a = 0.0f;
	else
	{
		pout = float4( pout.xyz * pin.color.xyz, pout.r * pin.color.a );
	}
	return pout;

}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 StaticMeshShader
{
    pass pass0
    {
        SetBlendState(AlphaBlendingState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );		
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }    
}
