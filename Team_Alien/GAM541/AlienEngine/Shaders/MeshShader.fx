#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};

cbuffer BoneTransforms
{
	float4 orient_bones[16];
	float3 pos_bones[16];
};

Texture2D diffuse;
Texture2D mask;

//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VSInputType
{
	float3 position0 : POSITION0;
	float3 position1 : POSITION1;
	float3 position2 : POSITION2;
	float3 position3 : POSITION3;
	float4 color : COLOR;
	float2 tex : TEXCOORD0;
	int4 vbones : BONES;
	float4 vweights : WEIGHTS;
	row_major float4x4 transform : TRANSFORM;
	uint flag : IFLAG;
};

struct GSInputType
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 tex : TEXCOORD0;
	int flag : FLAG;
};

struct PSInputType
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float3 view_position : VIEW;
	float2 tex : TEXCOORD0;
	int flag : FLAG;
};

struct PSOutputType
{
	float4 diffuse : SV_Target0;
	float4 normal_flag: SV_Target1;
	float4 view_position : SV_Target2;
};


float3 QuatRotatePoint( float4 quat, float3 position )
{
	float3 ret;
	float4 temp;
	temp.w = -(quat.x * position.x) - quat.y * position.y - quat.z * position.z;
	temp.x = quat.w * position.x + quat.y * position.z - quat.z * position.y;
	temp.y = quat.w * position.y + quat.z * position.x - quat.x * position.z;
	temp.z = quat.w * position.z + quat.x * position.y - quat.y * position.x;
	float4 inv;
	inv.w = quat.w; inv.x = -quat.x; inv.y = -quat.y; inv.z = -quat.z;
	ret.x = temp.x * inv.w + temp.w * inv.x + temp.y * inv.z - temp.z * inv.y;
	ret.y = temp.y * inv.w + temp.w * inv.y + temp.z * inv.x - temp.x * inv.z;
	ret.z = temp.z * inv.w + temp.w * inv.z + temp.x * inv.y - temp.y * inv.x;
	return ret;
}
//--------------------------------------------------------------------------------------
// VERTEX SHADER
//--------------------------------------------------------------------------------------
GSInputType VS( VSInputType vin )
{
	GSInputType vout;

	float3 pos = float3( 0.0f, 0.0f, 0.0f );
	if ( vin.vbones[0] < 0 )
	{
		pos = vin.position0;
	}
	else
	{
		float3 temp;
		temp = QuatRotatePoint( orient_bones[vin.vbones[0]], vin.position0 );
		temp += pos_bones[vin.vbones[0]];
		temp *= vin.vweights[0];
		pos += temp;
		//if ( vin.vbones[1] >= 0 )
		//{
			//temp = QuatRotatePoint( orient_bones[vin.vbones[1]], vin.position1 );
			//temp += pos_bones[vin.vbones[1]];
			//temp *= vin.vweights[1];
			//pos += temp;
			//if ( vin.vbones[2] >= 0 )
			//{
				//temp = QuatRotatePoint( orient_bones[vin.vbones[2]], vin.position2 );
				//temp += pos_bones[vin.vbones[2]];
				//temp *= vin.vweights[2];
				//pos += temp;
				//if ( vin.vbones[3] >= 0 )
				//{
					//temp = QuatRotatePoint( orient_bones[vin.vbones[3]], vin.position3 );
					//temp += pos_bones[vin.vbones[3]];
					//temp *= vin.vweights[3];
					//pos += temp;
				//}
			//}
		//}
	}
//	vout.position = mul( float4(vin.position,1.0f), vin.transform ).xyz;
	pos.yz = pos.zy;
//	pos.y *= 0.5f;
	vout.position = mul( float4(pos,1.0f), vin.transform ).xyz;

	vout.color = vin.color;
	vout.tex = vin.tex;
	vout.flag = vin.flag;

	return vout;
}
//--------------------------------------------------------------------------------------
// GEOMETRY SHADER
//--------------------------------------------------------------------------------------
[maxvertexcount(3)]
void GS(triangle GSInputType gin[3], inout TriangleStream<PSInputType> gout )
{
	PSInputType v0, v1, v2;

	v0.position = mul( float4(gin[0].position,1.0f), mat_view );
	v1.position = mul( float4(gin[1].position,1.0f), mat_view );
	v2.position = mul( float4(gin[2].position,1.0f), mat_view );
	v0.view_position = v0.position.xyz;
	v1.view_position = v1.position.xyz;
	v2.view_position = v2.position.xyz;
	float3 l01 = v1.view_position - v0.view_position;
	float3 l02 = v2.view_position - v0.view_position;
	float3 normal = cross( l01, l02 );
	normal = mul( normal, (float3x3)mat_view );
	normal = normalize(normal);
	v0.position = mul( v0.position, mat_proj );
	v1.position = mul( v1.position, mat_proj );
	v2.position = mul( v2.position, mat_proj );
	v0.normal = normal;
	v1.normal = normal;
	v2.normal = normal;
	v0.color = gin[0].color;
	v1.color = gin[1].color;
	v2.color = gin[2].color;
	v0.tex = gin[0].tex;
	v1.tex = gin[1].tex;
	v2.tex = gin[2].tex;
	v0.flag = gin[0].flag;
	v1.flag = gin[1].flag;
	v2.flag = gin[2].flag;
	gout.Append(v0);
	gout.Append(v1);
	gout.Append(v2);
}
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
	float4 textured = diffuse.Sample( sample_linear_wrap, pin.tex );
	textured.xyz *= textured.a;
	pout.diffuse.xyz = pin.color.xyz;
	pout.diffuse.xyz *= 1 - textured.a;
	pout.diffuse.xyz += textured.xyz;
	pout.diffuse.w = pin.color.w;
	return pout;
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 MeshShader
{
    pass pass0
    {
		SetBlendState( noblend, float4(0,0,0,0), 0xffffffff );
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
