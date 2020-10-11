/////////////
// GLOBALS //
/////////////
cbuffer PerFrameBuffer : register(b0)
{
	matrix mat_viewproj;
};
struct VSInputType
{
	float4x4 transform : MATRIX;
	float3 scaling : FLOAT0;
	float3 color : FLOAT1;
	int type : INTEGAR0;
	int flag : INTEGAR1;
};
struct GSInputType
{
	float4x4 transform : TRANSFORM;
	float3 scaling : SCALING;
	float3 color : COLOR;
	int type : TYPE;
	int flag : FLAG;
};
struct PSInputType
{
    float4 position : SV_POSITION;
	float4 color : COLOR;
	int flag : FLAG;
};
struct PSOutputType
{
	float4 diffuse : SV_TARGET0;
	float4 normal_depth : SV_TARGET1;
	uint flag : SV_TARGET2;
};

static const float pi = 3.1415927;

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
GSInputType VS(VSInputType vin)
{
    GSInputType vout;

	vout.transform = vin.transform;
	vout.scaling = vin.scaling;
	vout.color = vin.color;
	vout.type = vin.type;
	vout.flag = vin.flag;

    return vout;
}
////////////////////////////////////////////////////////////////////////////////
// Geometry Shader
////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(100)]
void GS( point GSInputType gin[1], inout TriangleStream<PSInputType>gout )
{
	PSInputType v;
	v.color = float4( gin[0].color, 1.0 );
	v.flag = gin[0].flag;

	if ( gin[0].type == 0 )
	{
		float4 p[8];
		float3 scaling = float3(1.0,1.0,1.0);//gin[0].scaling;
		//float3 offset = -gin[0].v1 / 2.0f;
		p[0].xyz = float3( 0, 0, 0 );
		p[0].xyz -= scaling * 0.5;

		p[1] = p[0];
		p[1].x += scaling.x;

		p[2] = p[1];
		p[2].z += scaling.z;
	
		p[3] = p[2];
		p[3].x -= scaling.x;

		p[4] = p[0];
		p[4].y += scaling.y;
	
		p[5] = p[4];
		p[5].x += scaling.x;

		p[6] = p[5];
		p[6].z += scaling.z;
	
		p[7] = p[6];
		p[7].x -= scaling.x;

		for ( int i = 0; i < 8; ++i )
		{
			p[i] = mul( float4( p[i].xyz, 1.0 ), gin[0].transform );
			p[i] = mul( p[i], mat_viewproj );
			//p[i] = mul( float4( p[i].xyz, 1.0 ), mat_viewproj );
		}
	
		v.position = p[0]; gout.Append(v); v.position = p[1]; gout.Append(v);
		v.position = p[3]; gout.Append(v); v.position = p[2]; gout.Append(v);
		v.position = p[7]; gout.Append(v); v.position = p[6]; gout.Append(v);
		v.position = p[4]; gout.Append(v); v.position = p[5]; gout.Append(v);
		v.position = p[0]; gout.Append(v); v.position = p[1]; gout.Append(v);
		gout.RestartStrip();
		v.position = p[2]; gout.Append(v); v.position = p[1]; gout.Append(v);
		v.position = p[6]; gout.Append(v); v.position = p[5]; gout.Append(v);
		gout.RestartStrip();
		v.position = p[0]; gout.Append(v); v.position = p[3]; gout.Append(v);
		v.position = p[4]; gout.Append(v); v.position = p[7]; gout.Append(v);
		gout.RestartStrip();
	}
	else if ( gin[0].type == 1 )
	{
		int n = 4;
		for ( int i = 0; i < n * 2; ++i )
		{
			float s0 = pi * (float)i / (float)n;
			float s1 = pi * (float)(i+1) / (float)n;
			for ( int j = 0; j <= n; ++j )
			{
				float t = pi * (float)j / (float)n;
				float x = sin( s0 ) * sin( t );
				float y = cos( t );
				float z = cos( s0 ) * sin( t );
				v.position = mul( float4( x, y, z, 1.0 ), gin[0].transform );
				v.position = mul( v.position, mat_viewproj );
				gout.Append(v);
				x = sin( s1 ) * sin( t );
				y = cos( t );
				z = cos( s1 ) * sin( t );
				v.position = mul( float4( x, y, z, 1.0 ), gin[0].transform );
				v.position = mul( v.position, mat_viewproj );
				gout.Append(v);
			}
			gout.RestartStrip();
		}
	}
	
}
////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
PSOutputType PS(PSInputType pin)
{
	//return float4( 0.0, 1.0, 1.0, 1.0 );
	PSOutputType pout;
	pout.diffuse = pin.color;
	pout.normal_depth = float4( 0.0, 0.0, 0.0, 0.0 );
	pout.flag = pin.flag;
    return pout;
}
