#include "states.fxh"

Texture1D random_texture;
uint global_time;
float elapsed_time;

float2 half_scale;
bool deactivating;
//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VertexType
{
	float3 v0 : POSITION0;
	float3 v1 : POSITION1;	// x-velocity, y-size, z-bloom
	float3 v2 : POSITION2;	// xy-previous position, z-timer
	uint type : VFLAG;
};

//--------------------------------------------------------------------------------------
// VERTEX SHADER
//--------------------------------------------------------------------------------------
VertexType VS( VertexType vin )
{
	return vin;
}
//--------------------------------------------------------------------------------------
// GEOMETRY SHADER
//--------------------------------------------------------------------------------------
[maxvertexcount(2)]
void GS( point VertexType gin[1], inout PointStream<VertexType> gout )
{
	VertexType v = gin[0];

	if ( v.type == 0 )
	{
		float4 r = GetRandom( random_texture, global_time, (int)v.v0.z );
		r -= 0.5;
		v.v0.x = r.x < 0.0? half_scale.x : -half_scale.x;
		v.v0.y = r.y < 0.0? half_scale.y : -half_scale.y;
		v.v1.xy = v.v0.xy;
		v.v2.xy = r.xy;
		v.v2.y *= 0.5;
		v.v2.xy = normalize( v.v2.xy );
		v.v2.xy *= 1.0 + 0.2 * ( r.z + r.w );
		v.v2.xy *= 5.0;

		v.type = 1;
		gout.Append(v);

	}
	else if ( v.type < 3 )
	{
		float mutation_rate = deactivating ? 0.00 : 0.06;
		float4 r = GetRandom( random_texture, global_time, (int)v.v0.z );
		r.xy -= 0.5;
		r.xy = normalize(r.xy);
		r.xy *= 0.08;

		VertexType newp = v;
		newp.v0.xy = v.v1.xy;
		newp.v0.xy += v.v0.xy;
		if ( v.type == 1 )
		{
			v.v1.xy += v.v2.xy * elapsed_time;
		}
		newp.v1.xy = v.v1.xy;
		newp.v1.xy += r.xy;
		newp.v2.x = 0.2;
		newp.v2.y = 2.0;
		newp.type = deactivating ? 4 : 3;

		v.v0.xy = r.xy;

		if ( abs( v.v1.x ) > half_scale.x )
		{
			if ( v.v1.x * v.v2.x > 0.0 ) v.v2.x = -v.v2.x;
			if ( v.type == 2 && r.z < mutation_rate ) v.type = 1;
			else if ( v.type == 1 && r.z > mutation_rate ) v.type = 2;
		}
		if ( abs( v.v1.y ) > half_scale.y )
		{
			if ( v.v1.y * v.v2.y > 0.0 ) v.v2.y = -v.v2.y;
			if ( v.type == 2 && r.z < mutation_rate ) v.type = 1;
			else if ( v.type == 1 && r.z > mutation_rate ) v.type = 2;
		}

		gout.Append(newp);
		gout.Append(v);

	}
	else
	{
		v.v2.x -= elapsed_time;
		v.v2.y -= elapsed_time * 10.0;
		if ( v.v2.x > 0.0 ) gout.Append(v);
	}

}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
GeometryShader SOGS = ConstructGSWithSO( 
						CompileShader( gs_4_0, GS() ), 
						"POSITION0.xyz; POSITION1.xyz; POSITION2.xyz; VFLAG.x" );
technique10 AdvanceParticles
{
	pass pass0
	{
		SetBlendState( noblend, float4(0,0,0,0), 0xffffffff );
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( SOGS );	
		SetPixelShader( NULL );
    }

}
