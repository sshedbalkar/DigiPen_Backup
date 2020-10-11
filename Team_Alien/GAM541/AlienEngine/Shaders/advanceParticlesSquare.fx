#include "states.fxh"

cbuffer PerFrameConstants
{
	uint global_time;
	float elapsed_time;
};

cbuffer EmitterConstants
{
	matrix mat_world;
	float3 initial_position_bottomleft;
	float3 initial_position_topright;
	float3 acceleration;
	float particle_radius;
	float3 initial_velocity;
	float3 velocity_deviation;
	float life_span;
	float life_span_deviation;
	bool deactivating;
};

Texture1D random_texture;
//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VertexType
{
	float3 position : POSITION0;
	float3 velocity : POSITION1;	// x-velocityX, y- velocityY, z - bloom
	float3 v2_timer : POSITION2;	// x-rotating, y-radius, z-timer
	uint type : VFLAG;
};

float4 GetRandom( int offset )
{
	float index = ((float)(global_time)) / 2130.0f + (float)offset / 100.0f;
	float4 ret = random_texture.SampleLevel( sample_point_wrap, index, 0 );
	return ret;
}
float3 GaussianRand( float3 expectation, float3 deviation, int offset )
{
	float3 r0 = GetRandom(offset).xyz;
	float3 r = GetRandom(2 * offset).xyz;

	r += r0;
	r -= 1.0f;

	return expectation + r * deviation;
}
float3 RandomVector( float3 lower, float3 higher, int offset )
{
	float3 r = GetRandom( offset ).xyz;

	r *= higher - lower;
	r += lower;
	return r;
}
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
[maxvertexcount(100)]
void GS( point VertexType gin[1], inout PointStream<VertexType> gout )
{
	VertexType v = gin[0];
	VertexType newp;
	v.v2_timer.z -= elapsed_time;

	if ( ( v.type & PT_LAUNCHER ) == PT_LAUNCHER )
	{
		if ( !deactivating )
		{
			if ( v.v2_timer.z < 0.0 || v.velocity.y > 0.9 )
			{
				int pcount = 0;
				if ( v.v2_timer.z < 0.0 )
				{
					pcount = (int)(v.velocity.x * (elapsed_time ) * 1000.0f );
					v.v2_timer.z = v.v2_timer.x;	/// reset timer for the launcher
				}
				if ( v.velocity.y > 0.9 )
				{
					pcount += (int)v.velocity.y;
					v.velocity.y = 0.0;
				}
				if ( pcount > 98 ) { v.velocity.y = (float)(pcount - 98); pcount = 98; }

				for ( int i = 0; i < pcount; ++i )
				{
					float4 r = GetRandom( i );
					newp.type = v.type - PT_LAUNCHER;
					newp.velocity.xy = GaussianRand( initial_velocity, velocity_deviation, i ).xy;
					newp.velocity.z = r.w;
					float speed = length(newp.velocity.xy);
					newp.velocity.xy = mul( newp.velocity.xy, (float2x2)mat_world );
					newp.velocity.xy = normalize(newp.velocity.xy);
					newp.velocity.xy *= speed;
					r.xyz *= initial_position_topright - initial_position_bottomleft;
					r.xyz += initial_position_bottomleft;
					r.xyz = mul( float4( r.xyz, 1.0f ), mat_world ).xyz;
					newp.position = r.xyz;
					if ( ( v.type & PT_RANDOMROTATION ) == PT_RANDOMROTATION )
						newp.v2_timer.x = r.w * 3.14f;	
					else newp.v2_timer.x = 0.0f;
					newp.v2_timer.y = particle_radius;
					newp.v2_timer.z = life_span + life_span_deviation * r.w;
					gout.Append(newp);
				}
			}
		}
		gout.Append(v);			/// put the launcher back into the pile
	}
	else	/// process particles
	{
		//if ( ( v.type & PT_GRAVITYAFFECTING ) == PT_GRAVITYAFFECTING ) 
			//v.velocity.y += gravity * elapsed_time;
		v.velocity.xy += acceleration.xy * elapsed_time;
		v.velocity.z += 0.2 * elapsed_time;
		if ( v.velocity.z > 2.0 ) v.velocity.z = 0.0;
		if ( ( v.type & PT_SHRINKING ) == PT_SHRINKING )
		{
			float t = v.v2_timer.z;
			t *= 0.9f; t /= life_span; t += 0.1f;
			if ( t > 1.0f ) t = 1.0f;
			v.v2_timer.y = t * particle_radius;
			//v.v2_timer.y = ( v.v2_timer.z / life_span ) * particle_radius;
		}
		if ( ( v.type & PT_ROTATING ) == PT_ROTATING )
			v.v2_timer.x += elapsed_time * 3.14f;
		v.position.xy += elapsed_time * v.velocity.xy;
		if ( v.v2_timer.z > 0.0f ) gout.Append(v);
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
