#include "states.fxh"

Texture1D random_texture;
uint global_time;
float elapsed_time;

float2 screen_range;
bool deactivating;
#define INTERVAL	0.0
#define RANGE_YZ	float2(5.0,120.0)
#define MARGIN		-40.0
#define SPACE		7.0
#define MAX_WIDTH	25.0
#define MIN_WIDTH	10.0
#define MIN_STROKE_RADIUS	0.3
#define MAX_STROKE_RADIUS	4.0
#define BRUSH_SPEED			10.0
#define BETWEEN_TWO_PARTICLES	0.3
#define VANISHING_RANGE		45.0
//--------------------------------------------------------------------------------------
//VERTEX AND PIXEL SHADER INPUTS
//--------------------------------------------------------------------------------------
struct VertexType
{
	float3 v0 : POSITION0;
	float3 v1 : POSITION1;
	float3 v2 : POSITION2;
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

	switch( v.type )
	{
	case CLOUDS_LAUNCHER:
		v.v2.x -= elapsed_time;
		v.v0.x = screen_range.y;
		if ( v.v2.x < 0 )
		{
			VertexType newp;
			bool k = false;
			if ( v.v1.y < screen_range.y - MARGIN )
			{
				if ( v.v1.y < screen_range.x ) v.v1.y = screen_range.x;
				if ( v.v1.x > screen_range.x ) v.v1.x = screen_range.x;
				newp.v0.x = v.v1.y;
				v.v1.y += SPACE;
				k = true;
			}
			else if ( v.v1.x > screen_range.x + MARGIN )
			{
				v.v1.x -= SPACE;
				newp.v0.x = v.v1.x;
				k = true;
			}
			if ( k )
			{
				float4 r = GetRandom( random_texture, global_time, 0 );
				newp.v1.x = r.x * ( MAX_WIDTH - MIN_WIDTH );	//width
				newp.v1.x += MIN_WIDTH;
				newp.v0.yz = r.yz;	//position
				newp.v0.yz -= 0.5;
				newp.v0.yz *= RANGE_YZ;
				newp.v0.yz += v.v0.yz;
				newp.v1.y = r.z * 0.4 + 0.5;//color intensity
				newp.v1.z = 0.0;		//timer
				newp.v2.x = r.w * MIN_STROKE_RADIUS;// stroke radius
				newp.v2.x += MIN_STROKE_RADIUS * 0.1;
				newp.v2.y = newp.v0.x;
				newp.v2.y += newp.v2.x * 0.5;//stroke center
				newp.v2.z = r.w * 0.6 + 0.2;//high point
				newp.type = CLOUDS_BRUSH_APPLYING;
				gout.Append(newp);
				v.v2.x = INTERVAL;
			}
		}
		gout.Append(v);
		break;

	case CLOUDS_BRUSH_APPLYING:
	
		v.v1.z += elapsed_time;
		if ( v.v1.z * BRUSH_SPEED > v.v1.x )
		{
			v.type = CLOUDS_BRUSH_APPLIED;
		}
		else
		{
			VertexType newp;
			float t = v.v1.z;
			t *= BRUSH_SPEED;
			newp.v0 = v.v0;
			newp.v0.x += t;
			newp.v1.x = pow(v.v2.x,2 );
			newp.v1.x -= pow( newp.v0.x - v.v2.y, 2 );
			if ( newp.v1.x < 0 ) newp.v1.x = 0.0;
			else newp.v1.x = sqrt( newp.v1.x );
			if ( newp.v1.x < v.v2.x * 0.5 && newp.v0.x > v.v2.y )//new stroke
			{
				float4 r = GetRandom( random_texture, global_time, 0 );
				t /= v.v1.x;
				if ( t < v.v2.z ) {	t /= v.v2.z; }
				else	{ t = 1.0 - t; t /= 1.0 - v.v2.z; }
				t *= MAX_STROKE_RADIUS - MIN_STROKE_RADIUS;
				t += MIN_STROKE_RADIUS;
				r.x -= 0.5;
				r.x *= t;
				r.x *= 0.5;
				t += r.x;
				v.v2.x = t;
				v.v2.y = newp.v0.x;
				v.v2.y += v.v2.x * 0.5;
			}
			newp.v1.yz = v.v1.yz;
			newp.v2.xy = v.v0.x;
			newp.v2.y += v.v1.x;
			newp.v2.z = v.v1.x;
			newp.v2.z -= ( MIN_WIDTH + MAX_WIDTH ) * 0.5;
			newp.v2.z /= MAX_WIDTH;
			newp.type = CLOUDS_PARTICLE;
			gout.Append( newp );
		}

		gout.Append(v);
		break;
	case CLOUDS_BRUSH_APPLIED:
		if ( v.v0.x > screen_range.y + VANISHING_RANGE 
			 || v.v0.x + v.v1.x < screen_range.x - VANISHING_RANGE )
		v.type = CLOUDS_BRUSH_INACTIVATED;
		gout.Append(v);
		break;
	case CLOUDS_BRUSH_INACTIVATED:
		if ( v.v0.x < screen_range.y - MARGIN 
			 && v.v0.x + v.v1.x > screen_range.x + MARGIN )
		{
			float4 r = GetRandom( random_texture, global_time, 0 );
			v.v1.z = 0.0;		//timer
			v.v2.x = r.w * MIN_STROKE_RADIUS;// stroke radius
			v.v2.x += MIN_STROKE_RADIUS * 0.5;
			v.v2.y = v.v0.x;
			v.v2.y += v.v2.x * 0.5;//stroke center
			v.v2.z = r.w * 0.6 + 0.2;//high point
			v.type = CLOUDS_BRUSH_APPLYING;
		}
		gout.Append(v);
		break;
	case CLOUDS_PARTICLE:
		v.v0.x += elapsed_time * v.v2.z;
		v.v2.xy += elapsed_time * v.v2.z;
		if ( v.v2.x > screen_range.y + VANISHING_RANGE 
			 || v.v2.y < screen_range.x - VANISHING_RANGE ) ;
		else gout.Append(v);
		break;
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
