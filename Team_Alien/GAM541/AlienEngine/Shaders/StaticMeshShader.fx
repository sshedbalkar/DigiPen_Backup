#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};

//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VSInputType
{
	float3 position : POSITION0;
	float3 normal : NORMAL;
	float4 color : COLOR;
	row_major float4x4 transform : TRANSFORM;
	uint flag : IFLAG;
};

struct PSInputType
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float3 view_position : VIEW;	
	uint flag : IFLAG;
};

struct PSOutputType
{
	float4 diffuse : SV_Target0;
	float4 normal_flag: SV_Target1;
	float4 view_position : SV_Target2;
};


//--------------------------------------------------------------------------------------
// VERTEX SHADER
//--------------------------------------------------------------------------------------
PSInputType VS( VSInputType vin )
{
	PSInputType vout;
	vout.view_position = mul( float4( vin.position, 1.0f ), vin.transform ).xyz;
	vout.view_position = mul( float4(vout.view_position,1.0f), mat_view ).xyz;
	vout.position = mul( float4(vout.view_position, 1.0f ), mat_proj );
	vout.position.xy /= vout.position.w;
	vout.view_position.z += 1.0;
	vout.position.zw = mul( float4( vout.view_position, 1.0 ), mat_proj ).zw;
	vout.position.xy *= vout.position.w;
	vout.normal = mul( vin.normal, (float3x3)vin.transform );
	vout.normal = mul( vin.normal, (float3x3)mat_view );
	vout.color = vin.color;
	vout.flag = vin.flag;

	return vout;
}
//--------------------------------------------------------------------------------------
// GEOMETRY SHADER(null)
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
PSOutputType PS( PSInputType pin )
{ 
	PSOutputType pout;

	pout.normal_flag.w = (float)pin.flag;
	pout.normal_flag.xyz = pin.normal;
	pout.view_position.xyz = pin.view_position;
	pout.view_position.w = 30.0f;
	pout.diffuse = pin.color;

	return pout;
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 StaticMeshShader
{
    pass pass0
    {
		SetBlendState( noblend, float4(0,0,0,0), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
