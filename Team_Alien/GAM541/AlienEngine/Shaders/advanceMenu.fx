#include "states.fxh"

cbuffer PerFrameConstants
{
	uint global_time;
	float elapsed_time;
};

Texture1D random_texture;
float2 cursor_position;
//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VertexType
{
	float3 position : POSITION0;
	float3 extra : POSITION1;
	float2 tex : TEXCOORD0;
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
	VertexType v;
	v = gin[0];
	if ( v.type < 2 ) // button
	{
		v.position.z += elapsed_time * v.extra.z;
		if ( v.position.z > 0.1 ) v.extra.z += elapsed_time * 200.0;
		else 
		{
			v.position.z = 0.0;
			v.extra.z = 0.0; 
			
			float2 dis = cursor_position;
			dis -= v.position.xy;
			dis = abs(dis);
			dis *= 2.0;
			if ( dis.x < v.extra.x && dis.y < v.extra.y )
			{
				v.type = 1;

				VertexType newp;
				newp.position.xy = v.position.xy;
				newp.position.z = 1.0;
				newp.tex = 0.0;
				newp.type = 2;
				float4 r = GetRandom( random_texture, global_time, 0 );
				r.xy -= 0.5;
				r.xy = normalize(r.xy);
				float2 absr = abs(r.xy);
				if ( absr.x > absr.y )
				{
					newp.position.x += ( 0.5 * v.extra.x - 10.0 ) * r.x / absr.x;
					newp.position.y += 0.5 * r.y * v.extra.y;
				}
				else 
				{
					newp.position.x += 0.5 * r.x * v.extra.x;
					newp.position.y += ( 0.5 * v.extra.y - 10.0 ) * r.y / absr.y;
				}
				newp.extra.xy = r.xy * 20.0;
				newp.extra.z = 20.0;
				gout.Append(newp);
			}
			else
			{
				v.type = 0;
			}
		}
		
		gout.Append(v);
	}
	else if ( v.type == 2 )
	{
		v.position.z -= elapsed_time;
		v.extra.z = 20.0 * v.position.z;
		v.position.xy += v.extra.xy * elapsed_time;
		if ( v.position.z > 0.0 ) gout.Append(v);
	}


}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
GeometryShader SOGS = ConstructGSWithSO( 
						CompileShader( gs_4_0, GS() ), 
						"POSITION0.xyz; POSITION1.xyz; TEXCOORD0.xy; VFLAG.x" );
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
