#include "states.fxh"


Texture1D random_texture;
float2 cursor_position;
uint global_time;
float timeslice;
//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VertexType
{
	float3 position0 : POSITION0;
	float3 position1 : POSITION1;
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
#define LIFE_TIME 0.1
[maxvertexcount(2)]
void GS( point VertexType gin[1], inout PointStream<VertexType> gout )
{
	VertexType v = gin[0];

	if ( v.type == 1 )
	{
		float4 r = GetRandom( random_texture, global_time, (int)v.position0.z );
		float2 offset = r.xy;
		offset -= 0.5;
		offset = normalize(offset);
		//offset *= r.z + r.w;

		v.position0.xy = v.position1.xy;
		v.position1.xy = cursor_position + offset * 7.0;

		if ( v.position1.z > 0.5 )
		{
			VertexType newp;
			newp.position0 = float3(v.position0.xy, 1.0);
			newp.position1 = float3( v.position1.xy, LIFE_TIME );
			float sql = pow(newp.position0.x - newp.position1.x,2.0) + pow(newp.position0.y - newp.position1.y,2.0);

			newp.type = 0;
			gout.Append(newp);
		}
		else v.position1.z += 0.25;
		gout.Append(v);
	}
	else
	{
		float sql = pow(v.position1.x - cursor_position.x,2.0) + pow(v.position1.y - cursor_position.y,2.0);
		v.position0.z -= 0.016;
		v.position1.z -= 0.016;
		if ( v.position1.z > 0.0 && sql < 10000.0 ) 
		{
			//v.position0.xy -= v.position1.xy;

			gout.Append(v);
		}
	}
}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
GeometryShader SOGS = ConstructGSWithSO( 
						CompileShader( gs_4_0, GS() ), 
						"POSITION0.xyz; POSITION1.xyz;VFLAG.x" );
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
