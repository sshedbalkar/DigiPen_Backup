#include "states.fxh"

cbuffer PerFrameConstants
{
	matrix mat_view;
	matrix mat_proj;
};

Texture2D diffuse_map;
Texture2D normal_map;
Texture2D mask;

//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VSInputType
{
	float3 position : POSITION0;
	float3 size : POSITION1;
	float3 extra : POSITION2;
	uint flag : VFLAG;
};

struct GSInputType
{
	float3 position : POSITION0;
	float3 size : POSITION1;
	float3 extra : POSITION2;
	uint flag : VFLAG;
};

struct PSInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 view_position : VIEW;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float2 extra : EXTRA;
	uint flag : VFLAG;
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
GSInputType VS( VSInputType vin )
{
	GSInputType vout;
	vout.position = vin.position;
	vout.size = vin.size;
	vout.extra = vin.extra;
	vout.flag = vin.flag;
	return vout;
}
//--------------------------------------------------------------------------------------
// GEOMETRY SHADER
//--------------------------------------------------------------------------------------
inline float3 ViewingTransform( float3 position )
{
	return mul( float4(position,1.0f), mat_view ).xyz;
}
inline float4 ProjectionTransform( float3 position )
{
	float4 projected = mul( float4( position, 1.0 ), mat_proj );
	projected.xy /= projected.w;
	position.z -= 2.0;
	projected.zw = mul( float4( position, 1.0 ), mat_proj ).zw;
	projected.xy *= projected.w;

	return projected;
}
inline void ProjectionCorrection( inout float3 v_position, inout float4 p_position, float2 zw_p, float z_v )
{
	p_position.xy /= p_position.w;
	p_position.xy *= zw_p.y;
	p_position.zw = zw_p;
	v_position.z = z_v;
}
#define SCALE_UV		float2(0.15,-0.15)
#define OFFSET			0.1
[maxvertexcount(8)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType> gout )
{
	PSInputType v;
	float3 position;
	float3 half_size = gin[0].size * 0.5;
	float2 scale_uv;
	float3 positions[4];
	if ( gin[0].extra.z < 0.0 )
	{
		scale_uv = gin[0].size.xy;
		scale_uv.x += OFFSET * 2.0;
	}
	else scale_uv = gin[0].extra.zz;
	scale_uv.y = -scale_uv.y;

	v.extra = gin[0].extra;
	v.flag = gin[0].flag;

	position = gin[0].position;
	position.xy -= half_size.xy;
	positions[0] = position;
	
	position.x += gin[0].size.x;
	positions[1] = position;
	
	position.x -= gin[0].size.x;
	position.y += gin[0].size.y;
	positions[2] = position;
	
	position.x += gin[0].size.x;
	positions[3] = position;
	
	position.z -= half_size.z;
	position = ViewingTransform( position );
	float z = position.z - 1.0;
	float2 zw = ProjectionTransform( position ).zw;
	float2 origin = positions[2].xy;
	origin.x -= OFFSET;
	
	//front surface
	v.normal = float3( 0, 0, -1 );
	v.tangent = float3( 1, 0, 0 );
	v.binormal = float3( 0, -1, 0 );
	v.normal = mul( v.normal, (float3x3)mat_view );
	v.tangent = mul( v.tangent, (float3x3)mat_view );
	v.binormal = mul( v.binormal, (float3x3)mat_view );
	
	v.tex = positions[0].xy - origin;
	v.tex /= scale_uv;
	v.view_position = ViewingTransform(positions[0]);
	v.position = ProjectionTransform(v.view_position);
	ProjectionCorrection( v.view_position, v.position, zw, z );
	gout.Append(v);
	
	v.tex = positions[1].xy - origin;
	v.tex /= scale_uv;
	v.view_position = ViewingTransform(positions[1]);
	v.position = ProjectionTransform(v.view_position);
	ProjectionCorrection( v.view_position, v.position, zw, z );
	gout.Append(v);
	
	v.view_position = positions[0];
	v.view_position.xy += OFFSET * float2( -1, 1 );
	v.tex = v.view_position.xy - origin;
	v.tex /= scale_uv;
	v.view_position = ViewingTransform(v.view_position);
	v.position = ProjectionTransform(v.view_position);
	ProjectionCorrection( v.view_position, v.position, zw, z );
	gout.Append(v);
	
	v.view_position = positions[1];
	v.view_position.xy += OFFSET * float2( 1, 1 );
	v.tex = v.view_position.xy - origin;
	v.tex /= scale_uv;
	v.view_position = ViewingTransform(v.view_position);
	v.position = ProjectionTransform(v.view_position);
	ProjectionCorrection( v.view_position, v.position, zw, z );
	gout.Append(v);

	v.view_position = positions[2];
	v.view_position.xy += OFFSET * float2( -1, -1 );
	v.tex = v.view_position.xy - origin;
	v.tex /= scale_uv;
	v.view_position = ViewingTransform(v.view_position);
	v.position = ProjectionTransform(v.view_position);
	ProjectionCorrection( v.view_position, v.position, zw, z );
	gout.Append(v);
	
	v.view_position = positions[3];
	v.view_position.xy += OFFSET * float2( 1, -1 );
	v.tex = v.view_position.xy - origin;
	v.tex /= scale_uv;
	v.view_position = ViewingTransform(v.view_position);
	v.position = ProjectionTransform(v.view_position);
	ProjectionCorrection( v.view_position, v.position, zw, z );
	gout.Append(v);

	v.tex = positions[2].xy - origin;
	v.tex /= scale_uv;
	v.view_position = ViewingTransform(positions[2]);
	v.position = ProjectionTransform(v.view_position);
	ProjectionCorrection( v.view_position, v.position, zw, z );
	gout.Append(v);
	
	v.tex = positions[3].xy - origin;
	v.tex /= scale_uv;
	v.view_position = ViewingTransform(positions[3]);
	v.position = ProjectionTransform(v.view_position);
	ProjectionCorrection( v.view_position, v.position, zw, z );
	gout.Append(v);
	gout.RestartStrip();

	
}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
PSOutputType PS( PSInputType pin )
{ 
	PSOutputType pout;

	// calculate normal
	float3 normal = 0.0;
	float3 bump = normal_map.Sample( sample_linear_wrap, pin.tex ).xyz;
	bump.xyz -= 0.5f;
	normal += bump.x * pin.tangent;
	normal += bump.y * pin.binormal;
	normal += (bump.z + 2.0) * pin.normal;
	pout.normal_flag.xyz = normal;
	pout.normal_flag.w = pin.flag;
	// store the diffuse color
	pout.diffuse = diffuse_map.Sample( sample_linear_wrap, pin.tex );
	pout.diffuse.w = pin.extra.y;
	// store the view position
	pout.view_position.xyz = pin.view_position;
	pout.view_position.w = 10;
	// masking
	float4 mask_texel = mask.Sample( sample_linear_wrap, pin.tex );
	if ( mask_texel.a > 0.95 )
	{
		pout.view_position.z -= 1.5;
		pout.diffuse.xyz = mask_texel;
	}
	return pout;
}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 TexturedBoxShader
{
	pass pass0
	{
		SetBlendState( noblend, float4(0,0,0,0), 0xffffffff );
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( CompileShader( gs_4_0, GS() ) );		
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }

}
