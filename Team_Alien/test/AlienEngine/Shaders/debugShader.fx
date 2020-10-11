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
	int type : BONE;
};

struct GSInputType
{
	float3 v0 : POSITION;
	float3 v1 : NORMAL;
	float4 color : COLOR;
	int type : BONE;
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
float2 AppendLineSegment( float3 start, float3 end, float4 color, inout TriangleStream<PSInputType> stream )
{
	PSInputType v;
	float thickness = 0.02f;
	float2 normal = start.xy - end.xy;
	normal.xy = normal.yx;
	normal.x = -normal.x;
	normal = normalize(normal);
	normal *= thickness;

	v.position = Transform( start - float3( normal.xy, 0.0f ) );
	v.color = color;
	stream.Append(v);
		
	v.position = Transform( start + float3( normal.xy, 0.0f ) );
	v.color = color;
	stream.Append(v);
		
	v.position = Transform( end - float3( normal.xy, 0.0f ) );
	v.color = color;
	stream.Append(v);

	v.position = Transform( end + float3( normal.xy, 0.0f ) );
	v.color = color;
	stream.Append(v);
	stream.RestartStrip();

	return normal / thickness;
}
///// 3d line segments
void AppendLineSegmentX( float3 start, float3 end, float4 color, inout TriangleStream<PSInputType> stream )
{
	PSInputType v;
	float thickness = 0.02f;

	v.position = Transform( start + float3( thickness, 0.0f, 0.0f ));
	v.color = color;
	stream.Append(v);
	v.position = Transform( start - float3( thickness, 0.0f, 0.0f ) );
	v.color = color;
	stream.Append(v);
	v.position = Transform( end + float3( thickness, 0.0f, 0.0f ) );
	v.color = color;
	stream.Append(v);
	v.position = Transform( end - float3( thickness, 0.0f, 0.0f ) );
	v.color = color;
	stream.Append(v);
	stream.RestartStrip();
}
///// 3d line segments
void AppendLineSegmentY( float3 start, float3 end, float4 color, inout TriangleStream<PSInputType> stream )
{
	PSInputType v;
	float thickness = 0.02f;

	v.position = Transform( start + float3( 0.0f, thickness, 0.0f ));
	v.color = color;
	stream.Append(v);
	v.position = Transform( start - float3( 0.0f, thickness, 0.0f ) );
	v.color = color;
	stream.Append(v);
	v.position = Transform( end + float3( 0.0f, thickness, 0.0f ) );
	v.color = color;
	stream.Append(v);
	v.position = Transform( end - float3( 0.0f, thickness, 0.0f ) );
	v.color = color;
	stream.Append(v);
	stream.RestartStrip();
}
[maxvertexcount(96)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType> gout )
{
	PSInputType v;
	/*0-line seg, 1-arrow, 2-rectangle, 3-circle, 4-prism*/
	/////////////////////////////////////
	////		line segment		/////
	/////////////////////////////////////
	if ( gin[0].type == 0 )
	{
		AppendLineSegment( gin[0].v0, gin[0].v1, gin[0].color, gout );
	}
	/////////////////////////////////////
	////			arrow			/////
	/////////////////////////////////////
	if ( gin[0].type == 1 )
	{
		float2 normal = AppendLineSegment( gin[0].v0, gin[0].v1, gin[0].color, gout );
		float3 direction = gin[0].v0 - gin[0].v1;
		float l = sqrt( pow(direction.x,2) + pow(direction.y,2) + pow(direction.z,2) );
		l *= 0.1f;
		normal *= l;
		normal *= 0.5f;

		v.position = Transform( gin[0].v1 );
		v.color = gin[0].color;
		gout.Append(v);

		v.position = Transform( gin[0].v1 + direction * 0.1f + float3( normal, 0.0f ) );
		v.color = gin[0].color;
		gout.Append(v);
		
		v.position = Transform( gin[0].v1 + direction * 0.1f - float3( normal, 0.0f ) );
		v.color = gin[0].color;
		gout.Append(v);
	}
	/////////////////////////////////////
	////		rectangle			/////
	/////////////////////////////////////
	if ( gin[0].type == 2 )
	{
		float3 v1,v2,v3;
		v1 = gin[0].v0;
		v1.x += gin[0].v1.x;
		v2 = v1;
		v2.y += gin[0].v1.y;
		v3 = gin[0].v0;
		v3.y += gin[0].v1.y;
		AppendLineSegmentY( gin[0].v0, v1, gin[0].color, gout );
		AppendLineSegmentX( v1, v2, gin[0].color, gout );
		AppendLineSegmentY( v2, v3, gin[0].color, gout );
		AppendLineSegmentX( v3, gin[0].v0, gin[0].color, gout );
	}
	/////////////////////////////////////
	////			circle			/////
	/////////////////////////////////////
	if ( gin[0].type == 3 )
	{
		float2 v0,v1;
		float z;
		int num_strip = 24;
		float angle = 6.2832f / (float)num_strip;
		float c = cos(angle), s = sin(angle);
		float2x2 rot;
		rot[0][0] = c; rot[0][1] = -s; rot[1][0] = s; rot[1][1] = c;
		v0 = gin[0].v0.xy;
		v0.x += gin[0].v1.x;
		z = gin[0].v0.z;

		for ( int i = 0; i < num_strip; ++i )
		{
			v1 = mul( v0, rot );
			AppendLineSegment( float3(v0,z), float3(v1,z), gin[0].color, gout );
			v0 = v1;
		}
	}
	/////////////////////////////////////
	////			prism			/////
	/////////////////////////////////////
	if ( gin[0].type == 4 )
	{
		float3 v[8];
		v[0] = gin[0].v0;
		v[1] = v[0]; v[1].x = gin[0].v1.x;
		v[2] = v[1]; v[2].z = gin[0].v1.z;
		v[3] = v[0]; v[3].z = gin[0].v1.z;
		v[4] = v[0]; v[4].y = gin[0].v1.y;
		v[5] = v[4]; v[5].x = gin[0].v1.x;
		v[6] = gin[0].v1;
		v[7] = v[4]; v[7].z = gin[0].v1.z;

		AppendLineSegmentY( v[0], v[1], gin[0].color, gout );
		AppendLineSegmentX( v[1], v[2], gin[0].color, gout );
		AppendLineSegmentY( v[2], v[3], gin[0].color, gout );
		AppendLineSegmentX( v[3], v[0], gin[0].color, gout );
		AppendLineSegmentX( v[0], v[4], gin[0].color, gout );
		AppendLineSegmentX( v[1], v[5], gin[0].color, gout );
		AppendLineSegmentX( v[2], v[6], gin[0].color, gout );
		AppendLineSegmentX( v[3], v[7], gin[0].color, gout );
		AppendLineSegmentY( v[4], v[5], gin[0].color, gout );
		AppendLineSegmentX( v[5], v[6], gin[0].color, gout );
		AppendLineSegmentY( v[6], v[7], gin[0].color, gout );
		AppendLineSegmentX( v[7], v[4], gin[0].color, gout );
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
technique10 StaticMeshShader
{
    pass pass0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );		
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }    
}