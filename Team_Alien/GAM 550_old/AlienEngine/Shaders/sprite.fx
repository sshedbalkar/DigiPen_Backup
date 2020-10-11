/////////////
// GLOBALS //
/////////////
cbuffer GSPerFrameBuffer : register(b0)
{
	matrix mat_viewproj;
};
cbuffer GSPerInstanceBuffer : register(b1)
{
	matrix mat_world;
};
Texture2D sprite_texture : register(t0);
SamplerState sampler_pc : register(s0);

struct VSInputType
{
    float2 position : FLOAT0;
	float2 size : FLOAT1;
	float4 tex : FLOAT2;
};
struct GSInputType
{
    float2 position : FLOAT0;
	float2 size : FLOAT1;
	float4 tex : FLOAT2;
};
struct PSInputType
{
    float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
GSInputType VS(VSInputType vin)
{
    GSInputType vout;
    
	vout.position = vin.position;
	vout.size = vin.size;
	vout.tex = vin.tex;

    return vout;
}
// transform function
inline float4 Transform( float3 pos )
{	
	float4 ret = mul( float4(pos, 1.0), mat_world );
	ret = mul( ret, mat_viewproj );
	return ret;
}
////////////////////////////////////////////////////////////////////////////////
// Geometry Shader
////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(4)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType>gout )
{
	PSInputType v;
	float2 halfsize = gin[0].size * 0.5;
	float2 halfsize_t = gin[0].tex.zw * 0.5;
	
	v.position = Transform( float3( gin[0].position - halfsize, 0.0 ) );
	v.tex = gin[0].tex.xy - halfsize_t;
	gout.Append(v);
	
	halfsize.y = -halfsize.y; halfsize_t.y = -halfsize_t.y;
	v.position = Transform( float3( gin[0].position - halfsize, 0.0 ) );
	v.tex = gin[0].tex.xy - halfsize_t;
	gout.Append(v); 
	
	halfsize = -halfsize; halfsize_t = -halfsize_t;
	v.position = Transform( float3( gin[0].position - halfsize, 0.0 ) );
	v.tex = gin[0].tex.xy - halfsize_t;
	gout.Append(v);
	
	halfsize.y = -halfsize.y; halfsize_t.y = -halfsize_t.y;
	v.position = Transform( float3( gin[0].position - halfsize, 0.0 ) );
	v.tex = gin[0].tex.xy - halfsize_t;
	gout.Append(v);

}
////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PS(PSInputType pin) : SV_TARGET
{
	//return float4( 0.0, 0.0, 0.0, 1.0 );
    return sprite_texture.Sample( sampler_pc, pin.tex );
}
