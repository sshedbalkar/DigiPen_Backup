/////////////
// GLOBALS //
/////////////
cbuffer PerFrameBuffer : register(b0)
{
	matrix mat_viewproj;
};
cbuffer BonesBuffer : register(b1)
{
	matrix mat_bones[80];
};

/////////////////////////////////// input/output structures for depth pass /////////////////////////////////
struct VSInputTypeAnimated
{
    float3 position : FLOAT0;
	uint4 indices : INTEGAR0;
	float4 weights : FLOAT1;
	int flag : INTEGAR1;
	float4x4 world : MATRIX;
};
struct VSInputTypeStatic
{
    float3 position : FLOAT0;
	int flag : INTEGAR0;
	float4x4 world : MATRIX;
};
struct PSInputType
{
	float4 position : SV_POSITION;
	float4 projected : POSITION;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shaders
////////////////////////////////////////////////////////////////////////////////
PSInputType VSAnimated(VSInputTypeAnimated vin)
{
	PSInputType vout;
	float4 position = float4( vin.position, 1.0 );
	vout.position = float4( 0.0, 0.0, 0.0, 0.0 );
	
	vout.position += mul( position, mat_bones[vin.indices.x] ) * vin.weights.x;
	vout.position += mul( position, mat_bones[vin.indices.y] ) * vin.weights.y;
	vout.position += mul( position, mat_bones[vin.indices.z] ) * vin.weights.z;
	vout.position += mul( position, mat_bones[vin.indices.w] ) * vin.weights.w;
	vout.position.w = 1.0;

	vout.position = mul( vout.position, vin.world );
	vout.position = mul( vout.position, mat_viewproj );
	vout.projected = vout.position;

    return vout;
}
////////////// vshader for static models /////////////////
PSInputType VSStatic(VSInputTypeStatic vin)
{
	PSInputType vout;
	vout.position = mul( float4( vin.position, 1.0 ), vin.world );
	vout.position = mul( vout.position, mat_viewproj );

    return vout;
}
////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PS(PSInputType pin) : SV_Target0
{
	return float4(0.0,0.0,0.0,0.0);
}
