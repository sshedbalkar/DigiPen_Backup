#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};


Texture2D color;			// {xyz_}-diffuse color
Texture2D normal_flag;		// {xyz_}-normal;	{___w} - flag(uint)
Texture2D view_position;	// {xyz_}-view position; {___w} - shininess(useless here)
Texture2D bloom4_texture;
Texture2D bloom8_texture;

float2 viewport;			// for calculating pixel width
float frametime;			// for accurate motion blur

Texture2D watermark_texture;

float2 camera_position;
float2 mouse_position;

//--------------------------------------------------------------------------------------
//	filter functions declaration
//--------------------------------------------------------------------------------------
float3 oilpainting( float2 tex );
inline int ShadowDetection( float2 dist, float depthM, float3 normalM, float2 tex );
inline void upscale( inout float3 color, float f );
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
	vout.size = 1.0f / viewport;
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

	v.position = Transform( float3(0.0f,0.0f,0.0f) );
	v.tex = float2( 0.0f,1.0f ) ;
	gout.Append(v);
	
	v.position = Transform( float3( gin[0].size.x, 0.0f, 0.0f ) );
	v.tex = float2( 1.0f,1.0f ) ;
	gout.Append(v);
	
	v.position = Transform( float3( 0.0f, gin[0].size.y, 0.0f ) );
	v.tex = float2( 0.0f, 0.0f );
	gout.Append(v);
	
	v.position = Transform( float3( gin[0].size, 0.0f ) );
	v.tex = float2( 1.0f, 0.0f ) ;
	gout.Append(v);
}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
float4 PS( PSInputType pin ) : SV_Target
{
	float4 result = float4(0,0,0,0);	//return value(RGBL)
 	float4 colorM = color.SampleLevel( sample_point_clamp, pin.tex, 0.0 );	//diffuse color
//	return colorM;
	float4 normalflag = normal_flag.SampleLevel( sample_point_clamp, pin.tex, 0.0 );
	float3 normalM = normalize(normalflag.xyz);
	uint flag = (uint)normalflag.a;
	float4 positionM = view_position.SampleLevel( sample_point_clamp, pin.tex, 0.0 );
	
	float4 bloom = bloom4_texture.SampleLevel( sample_linear_clamp, pin.tex, 0.0 );
//	return bloom;
	bloom += bloom8_texture.SampleLevel( sample_linear_clamp, pin.tex, 0.0 );
//	return bloom;
//************ deferred shading *****************//
// 1. diffuse shading
	if ( (flag & EFFECT_DIFFUSE) == EFFECT_DIFFUSE )
	{
		float intensity = 0.2f;
		/** calculate diffuse lighting **/
		// a sample directional light source is used here, could be changed due to game design
		float3 light_vec = float3( 0, 0.0, -1.0 );// - positionM.xyz;
		//light_vec = normalize( light_vec );
		intensity += saturate(dot( light_vec, normalM ));
//		result.xyz += float3( intensity.xxx * colorM.xyz );
		if ( (flag & EFFECT_CELSHADING ) == EFFECT_CELSHADING )
		{
//			intensity = CelShading(intensity);
		}
		result.xyz += intensity * colorM.xyz;
// 2. specular
		if ( (flag & EFFECT_SPECULAR ) == EFFECT_SPECULAR )
		{
			int shininess = (int)positionM.w;
			float3 eye_vec = -positionM.xyz;
			eye_vec = normalize(eye_vec);
			/// first specular light source
			float3 reflect_vec = reflect( -(float3( 0,-4,0) - positionM.xyz), normalM );
			reflect_vec = normalize( reflect_vec );
			result.xyz += float3(0.25,0.25,0.25) * pow( saturate( dot( reflect_vec, eye_vec ) ), shininess );
		}
	}
	else result = colorM;
	
	if ( ( flag & EFFECT_HALFBLOOM ) == EFFECT_HALFBLOOM ) result += 0.2 * bloom;
	else if ( ( flag & EFFECT_NONBLOOMING ) != EFFECT_NONBLOOMING ) result += bloom;
	result = saturate(result);
	//return result;


//******************* applying filters ********************//
	float depthM = positionM.z;
// 1. outline
	if ( (flag & EFFECT_OUTLINE ) == EFFECT_OUTLINE )
	{
		float thickness = 1.0f;
		float depthN = view_position.SampleLevel( sample_point_clamp, pin.tex + float2( 0.0,-thickness ) * viewport, 0.0 ).z;
		float depthW = view_position.SampleLevel( sample_point_clamp, pin.tex + float2(-thickness, 0.0 ) * viewport, 0.0 ).z;
		float depthS = view_position.SampleLevel( sample_point_clamp, pin.tex + float2( 0.0, thickness ) * viewport, 0.0 ).z;
		float depthE = view_position.SampleLevel( sample_point_clamp, pin.tex + float2( thickness, 0.0 ) * viewport, 0.0 ).z;
		
//		float min_depth = min( depthN, min( depthW, min( depthS, depthE ) ) );
		float max_depth = max( depthN, max( depthW, max( depthS, depthE ) ) );
	
//		float range_depth = max_depth - min_depth;
		float contrast_depth = max_depth / depthM;

		if ( contrast_depth > 1.01f ) result.xyz = float3(0.1,0.1,0.1);
	}
// 2. fake shadow
    else
	{
		float2 offset = float2( 1.2, -0.7 );
		int l = ShadowDetection( offset * 2.0, depthM, normalM, pin.tex );
		if ( l == 1 ) { result.xyz *= 0.1;  }
		else if ( l == 2 && ( ( flag & EFFECT_SHADOW ) == EFFECT_SHADOW ) ) { upscale( result.xyz, 0.5 ); }
	}
	//if ( depthM > 499.0 )
		//result.xyz = oilpainting( pin.tex );
//
//  cheap motion blur (blend with previous frame)
	//float t = saturate( MOTION_BLUR_FACTOR * frametime );
	//float4 prev = previous_frame.SampleLevel( sample_linear_clamp, pin.tex, 0.0 );
	//result *= t;
	//result += prev * ( 1.0f - t );
	//MotionBlur( result.xyz, pin.tex, 0.9 );
// final step : 
//  calculate and return RGBL value(for fxaa)
	result = saturate(result);
	//result.xyz = pow( result.xyz, 1.3 );
	result.a = luma(result);
	return result;
}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 PostProcessingShader
{
    pass pass0
    {
		SetBlendState( noblend, float4(0,0,0,0),0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );		
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }    
}
//--------------------------------------------------------------------------------------
// filter functions
//--------------------------------------------------------------------------------------


inline int ShadowDetection( float2 dist, float depthM, float3 normalM, float2 tex )
{
	int flag = (int)normal_flag.SampleLevel( sample_point_clamp, tex + dist * viewport, 0.0 ).w;
	float depthNE = view_position.SampleLevel( sample_point_clamp, tex + dist * viewport, 0.0 ).z;
	float contrast_depth = depthNE / depthM;
	if ( contrast_depth > 1.02 ) return 2;
	if ( ( flag & EFFECT_SHADOW ) != EFFECT_SHADOW ) return 0;
	if ( contrast_depth < 0.985 ) return 1;
	return 0;
}

inline void upscale( inout float3 color, float f )
{
	color += ( 1.0 - color ) * f;
}