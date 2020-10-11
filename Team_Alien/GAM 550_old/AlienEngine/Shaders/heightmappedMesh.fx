/////////////
// GLOBALS //
/////////////
cbuffer PerFrameBuffer : register(b0)
{
	matrix mat_viewproj;
};
cbuffer SizeBuffer : register(b1)
{
	float2 grid_size : packoffset( c0 );
	float height_scale : packoffset( c0.z );
	float area_scale : packoffset( c0.w );
};
struct VSInputType
{
    float2 position : FLOAT0;
};
struct PSInputType
{
    float4 position : SV_POSITION;
	float3 normal : NORMAL;
};
struct PSOutputType
{
	float4 diffuse : SV_TARGET0;
	float4 normal_depth : SV_TARGET1;
	uint flag : SV_TARGET2;
};

static const float gridsize = 1.0;

Texture2D height_map : register(t0);
SamplerState linearsampler : register(s0);

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PSInputType VS(VSInputType vin)
{
	PSInputType vout;
    
	float3 positions[5];

	positions[0] = float3( vin.position.x, 0, vin.position.y );
	positions[1] = float3( vin.position.x, 0, vin.position.y + 0.5 );
	positions[2] = float3( vin.position.x, 0, vin.position.y - 0.5 );
	positions[3] = float3( vin.position.x - 0.5, 0, vin.position.y );
	positions[4] = float3( vin.position.x + 0.5, 0, vin.position.y );

	for ( int i = 0; i < 5; ++i )
	{
		positions[i].y = height_map.SampleLevel( linearsampler, ( positions[i].xz + grid_size * 0.5 ) / grid_size, 0 ).x * height_scale;
		positions[i].y = 0;
		positions[i].xz *= area_scale;
	}

	vout.normal = cross( positions[2] - positions[1], positions[3] - positions[4] );
	vout.normal = normalize( vout.normal );
	vout.normal.x = 1 - vout.normal.x;
	vout.position = mul( float4( positions[0], 1.0 ), mat_viewproj );

    return vout;
}
////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
PSOutputType PS( PSInputType pin )
{
	PSOutputType pout;
	pout.diffuse = float4( 0.5, 0.4, 0.1, 0.2 );
	pout.normal_depth = float4( pin.normal, 0 );
	pout.flag = RENDER_FLAG_SHADED;
    return pout;
}
