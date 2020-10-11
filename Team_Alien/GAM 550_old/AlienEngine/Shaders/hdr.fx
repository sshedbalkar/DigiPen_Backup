#include "conversions.fxh"

cbuffer GSPerFrameBuffer : register(b0)
{
	matrix mat_ortho;
};

Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
Texture2D texture2 : register(t2);
Texture2D texture3 : register(t3);
Texture2D<uint> utexture0 : register(t4);
SamplerState sampler0 : register(s0);
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
	float2 screen : POSITION;
	float2 tex : TEXCOORD;
};
cbuffer BloomConstants : register( b0 )
{
	float2 tex_scale : packoffset(c0);
	float blur_range : packoffset(c0.z);
	float timeslice : packoffset(c0.w);
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

	v.screen = gin[0].screen;
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

inline float luma( float3 color )
{
	return max( dot( color, float3( 0.299, 0.587, 0.114 ) ), 0.0001 );
}
// Performs a gaussian blur in one direction
float3 Blur(in PSInputType input, float2 texScale, int texrange )
{
    float3 color = 0;
	float weightbase = ( (float)texrange + 1.0 );
	weightbase *= weightbase;
	weightbase += texrange * texrange * 2;
    for (int i = -texrange; i < texrange; ++i )
    {
		float weight = (float)( texrange + 1 - abs(i) ) + texrange;
		weight /= weightbase;
		float sigma = 20;
		weight = 1.0f / sqrt(2.0f * 3.14159 * sigma * sigma );
		weight *= exp(-( i * i ) / (2 * sigma * sigma ));
		//weight = 1.0 / (float)(texrange * 2);
        float2 texCoord = input.tex;
		texCoord += (float)i / input.screen * texScale;
		float4 sample = texture0.Sample( sampler0, texCoord );
		color += sample.xyz * weight;
    }

    return color;
}
//------------------------------------------------------------------------------------- output luma
float4 PSLuma( PSInputType pin ) : SV_Target
{ 
	float luminance = luma( texture0.Sample( sampler0, pin.tex ).xyz );
	luminance = log( max( luminance, 0.00000001 ) );
	return float4( luminance, 0.0, 0.0, 0.0 );
}
//------------------------------------------------------------------------------------- output luma
float4 PSAdaptation( PSInputType pin ) : SV_Target
{
    float lastLum = texture1.Load(uint3(0, 0, 0)).x;
    float avgLum = exp(texture0.SampleLevel(sampler0, float2(0.5f, 0.5f), 10.0f).x);

    float adaptedLum = lastLum + (avgLum - lastLum) * (1 - exp(- timeslice * 0.5));

    return float4(adaptedLum, 1.0f, 1.0f, 1.0f);
}
//------------------------------------------------------------------------------------- pick bright pixels
float4 PSHighlight( PSInputType pin ) : SV_Target
{
	float avgluma = exp( texture1.SampleLevel( sampler0, pin.tex, 10.0 ).x );
	float3 color = texture0.Sample( sampler0, pin.tex ).xyz;
	float luminance = luma( color );
	const float threshold = 1.5;
	if ( luminance - avgluma > threshold ) return float4( color, luminance / avgluma );
	return float4( 0,0,0,0 );
}
//------------------------------------------------------------------------------------- bloom
float4 PSBloom( PSInputType pin ) : SV_Target
{
	float4 result = float4( Blur( pin, tex_scale, (int)blur_range ), 1.0 );
	return result;
}
//------------------------------------------------------------------------------------- luma adaption&tone mapping
float4 PSToneMap( PSInputType pin ) : SV_Target
{ 
	float avgluma = texture1.Load( uint3(0,0,0) ).x;
	
	float4 result = texture0.Sample( sampler0, pin.tex );
	int2 texel = int2( pin.tex * pin.screen );
	uint flag = utexture0.Load( int3( texel, 0 ) ).x;

	float keyValue = 0.6;
	float linearExposure = (keyValue / avgluma );
	float exposure = log2(max(linearExposure, 0.0001f));
    //exposure -= threshold;
    result.xyz *= exp2(exposure);
	
	result.xyz += texture2.Sample( sampler0, pin.tex ).xyz * 0.2;
	//if ( luma( result.xyz ) > 1.0 ) result.xyz = normalize( result.xyz );


		//float avgluma = luma_map.SampleLevel( sstate, pin.tex, 120 ).x;
	result.xyz = saturate( result.xyz );

	if ( Flagged( flag, RENDER_FLAG_HIGHLIGHTED ) )
	{
		float3 highlight = float3( 0, 0, 0.2 );
		int width = 3;
		float depthM = texture3.Load( int3( texel, 0 ) ).x;
		
		uint flagN = utexture0.Load( int3( texel + int2( 0, -width ), 0 ) ).x;
		float depthN = texture3.Load( int3( texel + int2( 0, -width ), 0 ) ).x;
		if ( !Flagged( flagN, RENDER_FLAG_HIGHLIGHTED ) && depthN > depthM ) result.xyz = highlight;
		else
		{
			uint flagS = utexture0.Load( int3( texel + int2( 0, width ), 0 ) ).x;
			float depthS = texture3.Load( int3( texel + int2( 0, width ), 0 ) ).x;
			if ( !Flagged( flagS, RENDER_FLAG_HIGHLIGHTED ) && depthS > depthM ) result.xyz = highlight;
			else
			{
				uint flagW = utexture0.Load( int3( texel + int2( -width, 0 ), 0 ) ).x;
				float depthW = texture3.Load( int3( texel + int2( -width, 0 ), 0 ) ).x;
				if ( !Flagged( flagW, RENDER_FLAG_HIGHLIGHTED ) && depthW > depthM ) result.xyz = highlight;
				else
				{
					uint flagE = utexture0.Load( int3( texel + int2( width, 0 ), 0 ) ).x;
					float depthE = texture3.Load( int3( texel + int2( width , 0 ), 0 ) ).x;
					if ( !Flagged( flagE, RENDER_FLAG_HIGHLIGHTED ) && depthE > depthM ) result.xyz = highlight;

				}

			}
		}
	}

	result.w = luma( result.xyz );

	return result;
}
