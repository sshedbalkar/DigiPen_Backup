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
	float3 v0 : POSITION;
	float3 v1 : NORMAL;
	float4 color : COLOR;
	int type : VFLAG;
};

struct GSInputType
{
	float3 v0 : POSITION;
	float3 v1 : NORMAL;
	float4 color : COLOR;
	int type : FLAG;
};

struct PSInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//--------------------------------------------------------------------------------------
// VERTEX SHADER
//--------------------------------------------------------------------------------------
GSInputType VS( VSInputType vin )
{
	GSInputType vout;
	vout.v0 = vin.v0;
	vout.v1 = vin.v1;
	vout.color = vin.color;
	vout.type = vin.type;
	return vout;
}
//--------------------------------------------------------------------------------------
// GEOMETRY SHADER
//--------------------------------------------------------------------------------------
float4 Transform( float3 tin )
{
	float4 tout = float4( tin, 1.0f );
	tout = mul( tout, mat_view );
	tout = mul( tout, mat_proj );
	return tout;
}
////append 2d line segment function
void AppendLineSegment( float3 start, float3 end, float4 color, inout LineStream<PSInputType> stream )
{
	PSInputType v;

	v.position = Transform( start );
	v.color = color;
	stream.Append(v);
		
		
	v.position = Transform( end );
	v.color = color;
	stream.Append(v);

	stream.RestartStrip();
}
[maxvertexcount(25)]
void GS( point GSInputType gin[1], inout LineStream<PSInputType> gout )
{
	PSInputType v;
	/*0-line seg, 1-arrow, 2-rectangle, 3-circle, 4-prism*/
	/////////////////////////////////////
	////		line segment		/////
	/////////////////////////////////////
	if ( gin[0].type == 0 )
	{
		AppendLineSegment( gin[0].v0, gin[0].v0 + gin[0].v1, gin[0].color, gout );
	}
	/////////////////////////////////////
	////			arrow			/////
	/////////////////////////////////////
	if ( gin[0].type == 1 )
	{
		AppendLineSegment( gin[0].v0, gin[0].v0 + gin[0].v1, gin[0].color, gout );

		float3 direction = -gin[0].v1;
		float3 ending = gin[0].v0 + gin[0].v1;
		float l = sqrt( pow(direction.x,2) + pow(direction.y,2) + pow(direction.z,2) );
		l *= 0.1f;
		float2 normal = direction.yx;
		normal.x = -normal.x;
		normal *= l;
		normal *= 0.3f;

		v.position = Transform( ending );
		v.color = gin[0].color;
		gout.Append(v);

		v.position = Transform( ending + direction * 0.2f + float3( normal, 0.0f ) );
		v.color = gin[0].color;
		gout.Append(v);
		
		v.position = Transform( ending + direction * 0.2f - float3( normal, 0.0f ) );
		v.color = gin[0].color;
		gout.Append(v);
		
		v.position = Transform( ending );
		v.color = gin[0].color;
		gout.Append(v);
	}
	/////////////////////////////////////
	////		rectangle			/////
	/////////////////////////////////////
	if ( gin[0].type == 2 )
	{
		float3 v0,v1,v2,v3;
		float3 offset = float3( gin[0].v1.xy / 2.0f, 0.0f );
		v0 = gin[0].v0 + offset;
		offset.x = -offset.x;
		v1 = gin[0].v0 + offset;
		offset.y = -offset.y;
		v2 = gin[0].v0 + offset;
		offset.x = -offset.x;
		v3 = gin[0].v0 + offset;
		AppendLineSegment( v0, v1, gin[0].color, gout );
		AppendLineSegment( v1, v2, gin[0].color, gout );
		AppendLineSegment( v2, v3, gin[0].color, gout );
		AppendLineSegment( v3, v0, gin[0].color, gout );
	}
	/////////////////////////////////////
	////			circle			/////
	/////////////////////////////////////
	if ( gin[0].type == 3 )
	{
		PSInputType v;
		float2 p;
		float2 offset;
		float z;
		int num_strip = 24;
		float angle = 6.2832f / (float)num_strip;
		float c = cos(angle), s = sin(angle);
		float2x2 rot;
		rot[0][0] = c; rot[0][1] = -s; rot[1][0] = s; rot[1][1] = c;
		p = gin[0].v0.xy;
		z = gin[0].v0.z;
		offset = float2( gin[0].v1.x, 0.0f );

		for ( int i = 0; i <= num_strip; ++i )
		{
			v.position = Transform(float3(p + offset,z)) ;
			v.color = gin[0].color;
			gout.Append( v );

			offset = mul( offset, rot );
		}
	}
	/////////////////////////////////////
	////			prism			/////
	/////////////////////////////////////
	if ( gin[0].type == 4 )
	{
		float3 v[8];
		float3 offset = -gin[0].v1 / 2.0f;
		v[0] = gin[0].v0 + offset;
		offset.x = -offset.x;
		v[1] = gin[0].v0 + offset;
		offset.z = -offset.z;
		v[2] = gin[0].v0 + offset;
		offset.x = -offset.x;
		v[3] = gin[0].v0 + offset;
		offset.y = -offset.y;
		v[7] = gin[0].v0 + offset;
		offset.z = -offset.z;
		v[4] = gin[0].v0 + offset;
		offset.x = -offset.x;
		v[5] = gin[0].v0 + offset;
		offset.z = -offset.z;
		v[6] = gin[0].v0 + offset;

		AppendLineSegment( v[0], v[1], gin[0].color, gout );
		AppendLineSegment( v[1], v[2], gin[0].color, gout );
		AppendLineSegment( v[2], v[3], gin[0].color, gout );
		AppendLineSegment( v[3], v[0], gin[0].color, gout );
		AppendLineSegment( v[0], v[4], gin[0].color, gout );
		AppendLineSegment( v[1], v[5], gin[0].color, gout );
		AppendLineSegment( v[2], v[6], gin[0].color, gout );
		AppendLineSegment( v[3], v[7], gin[0].color, gout );
		AppendLineSegment( v[4], v[5], gin[0].color, gout );
		AppendLineSegment( v[5], v[6], gin[0].color, gout );
		AppendLineSegment( v[6], v[7], gin[0].color, gout );
		AppendLineSegment( v[7], v[4], gin[0].color, gout );
	}
}
//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
float4 PS( PSInputType pin ) : SV_Target
{ 
	return pin.color;
}
//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 DebugShader
{
    pass pass0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );		
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }    
}