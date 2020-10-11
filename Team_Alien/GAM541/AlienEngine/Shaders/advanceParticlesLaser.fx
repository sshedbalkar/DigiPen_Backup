#include "states.fxh"

Texture1D random_texture;
uint global_time;
float elapsed_time;

int state;
int colliding_side; //none/bottom/top/left/right
float2 start_point;
float2 end_point;
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
[maxvertexcount(3)]
void GS( point VertexType gin[1], inout PointStream<VertexType> gout )
{
	VertexType v = gin[0];

	///// root launcher
	if ( v.type == 0 )
	{
		if ( state > 0 )
		{
			VertexType newp;
			newp.v0.xy = start_point;
			newp.v1.xy = end_point;
			newp.v0.z = 0.0;
			newp.v1.z = (float)( global_time % 1000 ) + v.v0.x;
			newp.v2.z = 1.0;
			if ( state == 1 ) 
				newp.v2.xy = float2( 2.0, 12.0 );
			else if ( state == 2 )
				newp.v2.xy = float2( 12.0, 12.0 );
			newp.type = LASER_BEAM;
			gout.Append(newp);
		}
		v.v0.y -= elapsed_time;
		if ( v.v0.y < 0.0 && state > -1 )
		{
			float4 r = GetRandom( random_texture, global_time, (int)v.v0.x );
			float odds = state == 0 ? 0.06 : 0.4;
			v.v0.y = 0.2;
			if ( r.w < odds )
			{
				VertexType wave;
				wave.v0.xy = start_point;
				wave.v1.xy = start_point;
				wave.v2.xy = 0.0;
				wave.v1.z = v.v0.x;
				wave.v2.z = wave.v0.z = 0.0;
				wave.type = LASER_WAVE_LAUNCHER;
				gout.Append(wave);
			}
		}
		gout.Append(v);
	}
	// wave launcher
	else if ( v.type == LASER_WAVE_LAUNCHER )
	{
		v.v2.x += elapsed_time;
		if ( v.v2.x > 2.0 ) return;
		float4 r = GetRandom( random_texture, global_time, (int)v.v1.z );
		v.v0.xy = v.v1.xy;
		v.v1.xy = end_point;
		v.v1.xy -= v.v0.xy;
		if ( dot(v.v1.xy, end_point - start_point) < 0.0 )	// reflect the wave
		{
			if ( colliding_side == 0 ) return;
			VertexType newp = v;
			newp.v2.xy = start_point - end_point;
			newp.v2.xy = normalize( newp.v2.xy );
			if ( colliding_side < 3 ) newp.v2.x = -newp.v2.x;
			else newp.v2.y = -newp.v2.y;
			newp.v2.xy *= 3.0;
			newp.v1.xy = end_point;
			newp.v0.z = 0.3;
			newp.type = LASER_REFLECTED_WAVE_LAUNCHER;
			gout.Append(newp);

			float2 velocity = newp.v2.xy;
			float c,s;
			c = 0.5 + 0.5 * r.x;
			s = sqrt( 1 - c * c );
			if ( r.y > 0.5 ) s = -s;
			newp.v2.xy = float2( velocity.x * c - velocity.y * s, velocity.x * s + velocity.y * c );
			newp.v1.z += 2.0;
			gout.Append( newp );
			
			c = 0.5 + 0.5 * r.z;
			s = sqrt( 1 - c * c );
			if ( r.w > 0.5 ) s = -s;
			newp.v2.xy = float2( velocity.x * c - velocity.y * s, velocity.x * s + velocity.y * c );
			newp.v1.z += 2.0;
			gout.Append( newp );
		}
		else
		{
			v.v1.xy = normalize( v.v1.xy );
			v.v1.xy *= elapsed_time * 25.0;
			r.xy -= 0.5;
			r.xy = normalize( r.xy );
			r.xy *= 0.08;// wave randomness
			v.v1.xy += r.xy;
			v.v1.xy += v.v0.xy;

			VertexType newp = v;
			newp.type = LASER_WAVE;
			newp.v2.xy = 6.0;
			newp.v0.z = 0.3;
			gout.Append(newp);
			gout.Append(v);
		}
	}
	///
	else if ( v.type == LASER_REFLECTED_WAVE_LAUNCHER )
	{
		v.v0.z -= elapsed_time;
		if ( v.v0.z < 0.0 ) return;

		float4 r = GetRandom( random_texture, global_time, (int)v.v1.z );
		v.v0.xy = v.v1.xy;
		v.v1.xy = v.v2.xy * elapsed_time;
		r.xy -= 0.5;
		r.xy = normalize( r.xy );
		r.xy *= 0.04;
		v.v1.xy += r.xy;
		v.v1.xy += v.v0.xy;

		VertexType newp = v;
		newp.type = LASER_WAVE;
		newp.v0.z = 0.25;
		newp.v2.xy = 12.0;
		gout.Append(newp);
		gout.Append(v);
	}
	/// glowing beam
	else if ( v.type == LASER_BEAM )
	{
		float4 r = GetRandom( random_texture, global_time, (int)v.v1.z );
		r -= 0.5;
		r.xy = normalize( r.xy );
		r.zw = normalize( r.zw );
		v.v0.xy = start_point + r.xy * 0.015;
		v.v1.xy = end_point + r.zw * 0.025;

		v.v0.z += elapsed_time;
		if ( state < 1 ) v.v2.xy *= 0.93;
		if ( v.v0.z < 0.3 && state != -2 ) gout.Append(v);
	}
	/// electronic waves
	else if ( v.type == LASER_WAVE )
	{
		v.v0.z -= elapsed_time;
		if ( v.v0.z > 0.1 ) gout.Append(v);
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
