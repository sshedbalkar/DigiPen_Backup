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
Texture2D color : register(t0);
SamplerState sstate : register(s0);


/////////////////////////////////// input/output structures for diffuse pass /////////////////////////////////
struct VSInputTypeAnimated
{
    float3 position : FLOAT0;
	float3 normal : FLOAT1;
	float3 tangent : FLOAT2;
	float3 binormal : FLOAT3;
	float2 uv : FLOAT4;
	uint4 indices : INTEGAR0;
	float4 weights : FLOAT5;
	int flag : INTEGAR1;
	float4x4 world : MATRIX;
};
struct VSInputTypeStatic
{
    float3 position : FLOAT0;
	float3 normal : FLOAT1;
	float3 tangent : FLOAT2;
	float3 binormal : FLOAT3;
	float2 uv : FLOAT4;
	int flag : INTEGAR0;
	float4x4 world : MATRIX;
};
struct GSInputType
{
    float4 position : POSITION0;
	float3 view : POSITION1;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	int flag : FLAG;
};
struct PSInputType
{
    float4 position : SV_POSITION;
	float3 view : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	int flag : FLAG;
};
struct PSOutputType
{
	float4 diffuse : SV_TARGET0;
	float4 normal_depth : SV_TARGET1;
	uint flag : SV_TARGET2;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shaders
////////////////////////////////////////////////////////////////////////////////
GSInputType VSAnimated(VSInputTypeAnimated vin)
{
	GSInputType vout;
	float4 position = float4( vin.position, 1.0 );
	vout.position = float4( 0.0, 0.0, 0.0, 0.0 );
	vout.normal = float3( 0.0, 0.0, 0.0 );
	
	//vout.position += mul( position, mat_bones[vin.indices.x] );
	//vout.position = position;
	vout.position += mul( position, mat_bones[vin.indices.x] ) * vin.weights.x;
	vout.position += mul( position, mat_bones[vin.indices.y] ) * vin.weights.y;
	vout.position += mul( position, mat_bones[vin.indices.z] ) * vin.weights.z;
	vout.position += mul( position, mat_bones[vin.indices.w] ) * vin.weights.w;
	vout.position.w = 1.0;

	vout.normal += mul( vin.normal, (float3x3)mat_bones[vin.indices.x] ) * vin.weights.x;
	vout.normal += mul( vin.normal, (float3x3)mat_bones[vin.indices.y] ) * vin.weights.y;
	vout.normal += mul( vin.normal, (float3x3)mat_bones[vin.indices.z] ) * vin.weights.z;
	vout.normal += mul( vin.normal, (float3x3)mat_bones[vin.indices.w] ) * vin.weights.w;

	vout.position = mul( vout.position, vin.world );
	vout.view = vout.position.xyz;
	vout.position = mul( vout.position, mat_viewproj );

	vout.normal = mul( vout.normal, (float3x3)vin.world );
	vout.uv = vin.uv;
	
	vout.flag = vin.flag;
    return vout;
}
////////////// vshader for static models /////////////////
GSInputType VSStatic(VSInputTypeStatic vin)
{
	GSInputType vout;
	vout.position = mul( float4( vin.position, 1.0 ), vin.world );
	vout.view = vout.position.xyz;
	vout.position = mul( vout.position, mat_viewproj );

	vout.normal = mul( vin.normal, (float3x3)vin.world );
	vout.uv = vin.uv;
	
	vout.flag = vin.flag;
    return vout;
}
////////////////////////////////////////////////////////////////////////////////
// Geometry Shader
////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(3)]
void GS( triangle GSInputType gin[3], inout TriangleStream<PSInputType>gout )
{
	PSInputType v;
	v.flag = gin[0].flag;
	v.position = gin[0].position;
	v.normal = gin[0].normal;
	v.uv = gin[0].uv;
	v.view = gin[0].view;
	gout.Append(v);
	v.position = gin[1].position;
	v.normal = gin[1].normal;
	v.uv = gin[1].uv;
	v.view = gin[1].view;
	gout.Append(v);
	v.position = gin[2].position;
	v.normal = gin[2].normal;
	v.uv = gin[2].uv;
	v.view = gin[2].view;
	gout.Append(v);
}
////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
PSOutputType PS(PSInputType pin)
{
	PSOutputType pout;
	pout.diffuse = color.Sample( sstate, pin.uv );
	pout.diffuse.w = 0.05;
	pout.normal_depth.xyz = pin.normal;
	pout.normal_depth.w = pin.position.w;
	pout.flag = pin.flag;
	return pout;
}
