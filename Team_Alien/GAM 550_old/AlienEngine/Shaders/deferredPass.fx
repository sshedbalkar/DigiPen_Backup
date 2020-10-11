#define MAX_LIGHTSPOINT		16
#define MAX_LIGHTSSPOT		16

#include "conversions.fxh"

cbuffer GSPerFrameBuffer : register(b0)
{
	matrix mat_ortho;
};
cbuffer PSPerFrameBuffer : register( b0 )
{
	float4x4 mat_viewproj_inv : packoffset(c0);
};
cbuffer PSLightMainBuffer : register(b1)
{
	float3 eyews : packoffset(c0);
	float3 lddir : packoffset(c1);
};
//cbuffer PSLightPointBuffer : register(b1)
//{
	//float4 lppos[MAX_LIGHTSPOINT] : packoffset(c0);
	//float4 lpclr[MAX_LIGHTSPOINT] : packoffset(c16);
//};
cbuffer Temp : register(b2)
{
	float4x4 mat_vp : packoffset(c0);
};

Texture2D diffuse_roughness : register(t0);
Texture2D normal_depth : register(t1); /// {xyz-normal, w-depth}
Texture2D<uint> flag_map : register(t2);
Texture2D depth_map : register(t3);
Texture2D shadow_map : register(t4);
//Texture2D shade : register(t5);
SamplerState pointsampler : register(s0);
SamplerState linearsampler : register(s1);
SamplerComparisonState shadowsampler : register(s2);

static const float ambient = 0.3;
static const float3 lightcolor = float3( 1.0, 1.0, 1.0 );
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
//--------------------------------------------------------------------------------------
// lighting models
//--------------------------------------------------------------------------------------
//--- cook-torrance lighting model --------------------------------------
float3 CookTorrance( float3 position, float3 normal, float3 albedo, float roughness_ref )
{
	float3 vlight = lddir;
	float3 vview = normalize( eyews - position );
	vlight = normalize(vlight);
	float3 vhalf = vview + vlight;
	vhalf = normalize(vhalf);
	float NdotL = clamp( dot( normal, vlight ), 0.0001, 1.0 );
	float NdotH = clamp( dot( normal, vhalf ), 0.0001, 1.0 );
	float NdotV = clamp( dot( normal, vview ), 0.0001, 1.0 );
	float VdotH = clamp( dot( vview, vhalf ), 0.0001, 1.0 );
	float r_sq = roughness_ref;
	//---- evaluate the geometric term -------------------------------------------
	float geo0 = 2 * NdotH * NdotV; geo0 /= VdotH;
	float geo1 = 2 * NdotH * NdotL; geo1 /= VdotH;
	float geo = min( 1.0, min( geo0, geo1 ) );
	//---- the roughness term ----------------------------------------------------
	float roughness_a = 1.0 / ( 4.0 * r_sq * pow( NdotH, 4 ) );
	float roughness_b = NdotH * NdotH - 1.0;
	float roughness_c = r_sq * NdotH * NdotH;
	float roughness = roughness_a * exp( roughness_b / roughness_c );
	//---- the fresnel term ------------------------------------------------------
	float ref = pow( 1 - roughness_ref, 10 );
	float fresnel = pow( 1.0 - VdotH, 5.0 );
	fresnel *= ( 1.0 - ref );
	fresnel += ref;
	//---- put together to get the specular intensity ----------------------------
	float rs = fresnel; rs *= geo; rs *= roughness; rs /= NdotV; rs /= NdotL;
	rs = clamp( rs, 0.0, 1.0 );
	//rs *= 0.1;
	return max( ambient, NdotL ) * ( albedo + rs * 3.0 ) * lightcolor;
}
//--- blinn-phong lighting model --------------------------------------------------
float3 BlinnPhong( float3 position, float3 normal, float3 diffuse )
{
	float3 vlight = lddir;
	float3 vview = normalize( eyews - position );
	vlight = normalize(vlight);
	float3 vhalf = vview + vlight;
	vhalf = normalize(vhalf);
	float NdotL = clamp( dot( normal, vlight ), 0.0001, 1.0 );
	float NdotH = clamp( dot( normal, vhalf ), 0.0001, 1.0 );
	
	return max( ambient, NdotL ) * diffuse + pow( NdotH, 100 );
}
//-------------------------------------------------------------------------------------------- ward
float3 Ward( float3 position, float3 normal, float3 diffuse )
{
	float3 n = normalize( normal );
	float3 l = normalize( lddir );
	float3 v = normalize( eyews - position );
	float3 h = normalize( l + v );

	float2 roughness = float2( 0.1, 0.1 );

	float3 epsilon = float3( 1.0, 0.0, 0.0 );
	float3 tangent = normalize( cross( n, epsilon ) );
	float3 bitangent = normalize( cross( n, tangent ) );

	float VdotN = clamp( dot( v, n ), 0.0001, 1.0 );
	float LdotN = clamp( dot( l, n ), 0.0001, 1.0 );
	float HdotN = dot( h, n );
	float HdotT = dot( h, tangent );
	float HdotB = dot( h, bitangent );

	float beta_a = HdotT / roughness.x;
	beta_a *= beta_a;
	float beta_b = HdotB / roughness.y;
	beta_b *= beta_b;
	float beta = -2.0 * ( ( beta_a + beta_b ) / ( 1.0 + HdotN ) );

	float s_den = 4.0 * 3.1415927;
	s_den *= roughness.x;
	s_den *= roughness.y;
	s_den *= sqrt( LdotN * VdotN );

	float3 spec = exp(beta) / s_den;
	spec = saturate( spec );

	return max( ambient, LdotN ) * ( diffuse + spec * 3.0 ) * lightcolor;
 
}
float3 Strokes( float3 position, float2 tex )
{
	
//	return shade.Sample( linearsampler, float2( HdotT, HdotB ) ).xyz;
	return 0.0;
}
//---- shadow calculations --------------------------------------------------------
float ShadowVisibility( float3 _positionWS )
{
	float4 projected = mul( float4( _positionWS, 1.0), mat_vp );
	projected.xyz /= projected.w;
	projected.y = -projected.y;
	projected.xy *= 0.5;
	projected.xy += 0.5;
	float d = shadow_map.SampleCmp( shadowsampler, projected.xy, projected.z - 0.005 ).x;
	return d;
}
//----------------------------------------------------------------------------------- position reconstruction
float3 ReconstructPositionWS( float depth, float2 positionSS )
{
	positionSS -= 0.5;
	positionSS *= 2.0;
	float4 position = float4( positionSS.x, -positionSS.y, depth, 1.0 );
	position = mul( position, mat_viewproj_inv );
	position /= position.w;
	return position.xyz;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//	entry points
/////////////////////////////////////////////////////////////////////////////////////////////////////////
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
float4 PS( PSInputType pin ) : SV_Target
{
	uint flag = flag_map.Load( int3( int2(pin.tex * pin.screen), 0 ) ).x;
	float depth = depth_map.Sample( pointsampler, pin.tex ).x;
//	return float4( depth.xxx, 1.0 );
	float3 position = ReconstructPositionWS( depth, pin.tex );
	//position = flags.xyz;
	float4 diffuse_r = diffuse_roughness.Sample( pointsampler, pin.tex );
	float4 nd = normal_depth.Sample( pointsampler, pin.tex );
	nd.xyz = normalize(nd.xyz);
	float4 result;

	//--- lighting on -----------------------------------------------------------------
	if ( Flagged( flag, RENDER_FLAG_SHADED ) )
	{
		float shadow = ShadowVisibility( position );
		/// the result color is a linear combination of diffuse+specular lighting and ambient lighting
		result.xyz = CookTorrance( position, nd.xyz, diffuse_r.xyz, diffuse_r.w ) * shadow;
		//result.xyz = Ward( position, nd.xyz, diffuse_r.xyz );// * shadow;
		result.xyz += ambient * diffuse_r.xyz * ( 1 - shadow ) * lightcolor;
		//result.xyz += CookTorrance( position, nd.xyz, diffuse_r.xyz, -lddir, float3( 0.1, 0.1, 0.1) );// * shadow;
	}
	else result.xyz = diffuse_r.xyz;

	
//
//
    //result.xyz = max(0, result.xyz - 0.004f);
    //result.xyz = (result.xyz * (6.2f * result.xyz + 0.5f)) / (result.xyz * (6.2f * result.xyz + 1.7f)+ 0.06f);	
	//result.xyz = pow( result.xyz, 2.2 );
	//result.w = luma( result.xyz );
	return result;
}
