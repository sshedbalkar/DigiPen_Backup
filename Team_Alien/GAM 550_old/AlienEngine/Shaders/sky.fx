/////////////
// GLOBALS //
/////////////
cbuffer VSPerFrameBuffer : register(b0)
{
	matrix mat_view;
	matrix mat_proj;
};
cbuffer PSPerFrameBuffer : register(b0)
{
	float2 translation : packoffset(c0);
	float noise_scale : packoffset(c0.z);
	float brightness : packoffset(c0.w);
};

struct VSInputType
{
    float3 position : FLOAT0;
	float2 tex : FLOAT1;
};
struct PSInputType
{
	float3 positionWS : POSITION;
    float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};
struct PSOutputType
{
	float4 diffuse : SV_TARGET0;
	float4 normal_depth : SV_TARGET1;
	uint flag : SV_TARGET2;
};

Texture2D noisetexture : register(t0);
Texture2D cloudtexture : register(t1);
SamplerState linear_sampler : register(s0);

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PSInputType VS(VSInputType vin)
{
    PSInputType vout;
    
	vout.positionWS = vin.position;
	float3 positionVS = mul( vin.position, (float3x3)mat_view );
	vout.position = mul( float4( positionVS, 1.0 ), mat_proj );
	vout.tex = vin.tex;

    return vout;
}

////////////////////////////////////////////////////////////////////////////////
// clear sky model
////////////////////////////////////////////////////////////////////////////////
float AngleBetween(in float3 dir0, in float3 dir1)
{
	return acos(dot(dir0, dir1));
}
float3 CIEClearSky(in float3 dir, in float3 sunDir)
{
	float3 skyDir = float3(dir.x, abs(dir.y), dir.z);
	float gamma = AngleBetween(skyDir, sunDir);
	float S = AngleBetween(sunDir, float3(0, 1, 0));
	float theta = AngleBetween(skyDir, float3(0, 1, 0));

	float cosTheta = cos(theta);
	float cosS = cos(S);
	float cosGamma = cos(gamma);

	float num = (0.91f + 10 * exp(-3 * gamma) + 0.45 * cosGamma * cosGamma) * (1.0 - exp(-0.3 / cosTheta));
	float denom = (0.91f + 10 * exp(-3 * S) + 0.45 * cosS * cosS) * (1 - exp(-0.32f));

	float lum = num / denom;

	// Clear Sky model only calculates luminance, so we'll pick a strong blue color for the sky
	const float3 SkyColor = float3(0.2f, 0.5f, 1.0f) ;
	const float3 SunColor0 = float3(1.0f, 0.8f, 0.3f) * 150;
	const float3 SunColor1 = float3(1.0f, 0.8f, 0.3f) * 1.5;

	const float SunWidth0 = 0.05f;
	const float SunWidth1 = 0.1f; 

	float3 color = SkyColor;

	// Draw a circle for the sun
	float sunGamma = AngleBetween(dir, sunDir);
	//if ( sunGamma < SunWidth ) color = SunColor;
	color += lerp(SunColor0, SunColor1, saturate(abs(sunGamma) / SunWidth0));
	if ( abs(sunGamma) > SunWidth0 )
	{
		color = lerp( SunColor1, SkyColor, saturate( (abs(sunGamma) - SunWidth0) / SunWidth1 ) );
	}

	//return color;
	return max(color * lum, 0);
}
////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PS(PSInputType pin) : SV_TARGET0
{	
	PSOutputType pout;
    // Translate the texture coordinate sampling location by the translation value.
    pin.tex.x += translation.x;

    // Sample the texture value from the perturb texture using the translated texture coordinates.
    float2 perturb = noisetexture.Sample( linear_sampler, pin.tex ).xx;

    // Multiply the perturb value by the perturb scale.
    perturb *= noise_scale;

    // Add the texture coordinates as well as the translation value to get the perturbed texture coordinate sampling location.
    perturb += pin.tex + translation;

    // Now sample the color from the cloud texture using the perturbed sampling coordinates.
	float3 color = CIEClearSky( normalize( pin.positionWS ), normalize(float3( 0, 1, 1 )) ) ;
	
	float cloudintensity = cloudtexture.Sample( linear_sampler, perturb ).x * 1.1;
	const float3 cloudcolor = float3( 1.0, 1.0, 1.0 );
	//if ( cloudintensity > 0.8 ) cloudintensity = 1.0;
	if ( pin.positionWS.y > 0.0 ) 
	{
		color *= saturate( 1 - cloudintensity );
		color += saturate(cloudintensity) * cloudcolor;
	}

    // Reduce the color cloud by the brightness value.
    //cloudColor = cloudColor * brightness;

    return float4( color, 1.0 );
}
