#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};

Texture2D blur_source;
float blur_range;
float brightness;
float2 viewport;

float3 oilpainting( float2 tex, float brush_width );
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
	
	v.position = Transform( float3( 0.0, gin[0].size.y, 0.0f ) );
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
	float4 result = 0;
	
	result.xyz = oilpainting( pin.tex, blur_range );

	float b = blur_range / 6.0f;
	float bwness = b * 0.7;
	float bwcolor = dot( result.xyz, 0.3333 );
	result.xyz *= 1 - bwness;
	result.xyz += bwness * bwcolor;
	if ( brightness > 0.0 )
	{
		result.xyz *= 1.0 - brightness;
		result.xyz += brightness;
	}
	else
	{
		result.xyz *= 1.0 + brightness;
	}
	//result.xyz += ( b ) * 1.0;

	return result;

}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 BlurShader
{
    pass pass0
    {
		SetBlendState( noblend, float4(0,0,0,0),0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );		
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }    
}

#define LevelOfIntensity		7
float3 oilpainting( float2 tex, float brush_width )
{
	float i,j,stepij;
	float2 offset = float2(0.0f,0.0f);
	float3 average[LevelOfIntensity];
	int pixel_count[LevelOfIntensity];
	for ( int i = 0; i < LevelOfIntensity; ++i ) { average[i]=float3(0,0,0); pixel_count[i] = 0; }
	int max_count = 0;
	int max_index = 0;
	i = -brush_width;
	stepij = brush_width * 0.3;
	while ( i <= brush_width )
	{
		float rj = brush_width - abs(i);
		j = -rj;
		while( j <= rj )
		{
			offset = float2( i, j ) * viewport;
			float4 colorij = blur_source.SampleLevel( sample_linear_clamp, tex + offset, 0.0 );
			int intensity = (int)((float)LevelOfIntensity * dot( colorij.xyz, 0.333 ));
			//--intensity;
			++pixel_count[intensity];
			average[intensity] += colorij.rgb;
			if ( pixel_count[intensity]>= max_count ) { max_index = intensity; max_count = pixel_count[intensity]; }
			j += 1.0;
		}
		i += 1.0;
	}
//	if ( pixel_count[4] == 0 ) return float3(1,1,1);
	return average[max_index] / (float)max_count;//pixel_count[3];
}