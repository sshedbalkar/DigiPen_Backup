/////////////
// GLOBALS //
/////////////
cbuffer PerFrameBuffer : register(b0)
{
	matrix mat_viewproj;
};
struct VSInputType
{
	float3 f0 : FLOAT0;
	float3 f1 : FLOAT1;
	float4 color : FLOAT2;
	int type : INTEGAR0;
};
struct GSInputType
{
	float3 f0 : POSITION0;
	float3 f1 : POSITION1;
	float4 color : COLOR;
	int type : TYPE;
};
struct PSInputType
{
    float4 position : SV_POSITION;
	float4 color : COLOR;
};

static const float pi = 3.1415927;

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
GSInputType VS(VSInputType vin)
{
    GSInputType vout;

	vout.f0 = vin.f0;
	vout.f1 = vin.f1;
	vout.color = vin.color;
	vout.type = vin.type;

    return vout;
}
////////////////////////////////////////////////////////////////////////////////
// Geometry Shader
////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(10)]
void GSSolid( point GSInputType gin[1], inout TriangleStream<PSInputType>gout )
{
	PSInputType v;
	v.color = gin[0].color;

	if ( gin[0].type == DEBUG_SHAPE_BOX )
	{
		float4 position = float4( gin[0].f0, 1.0 );
		float2 scaling = gin[0].f1.xy;

		position.xy -= scaling * 0.5;
		v.position = mul( position, mat_viewproj );
		gout.Append(v);
		position.x += scaling.x;
		v.position = mul( position, mat_viewproj );
		gout.Append(v);
		position.x -= scaling.x;
		position.y += scaling.y;
		v.position = mul( position, mat_viewproj );
		gout.Append(v);
		position.x += scaling.x;
		v.position = mul( position, mat_viewproj );
		gout.Append(v);
	}
	else if ( gin[0].type == DEBUG_SHAPE_ARROW )
	{
		const float stick_width = 0.001;
		const float arrow_size = 0.01;
		float4 position0 = float4( gin[0].f0, 1.0 );
		float4 position1 = float4( gin[0].f1, 1.0 );
		float2 direction = normalize( float2( gin[0].f1.x - gin[0].f0.x, gin[0].f1.y - gin[0].f0.y ) );
		float2 normal = normalize( float2( gin[0].f0.y - gin[0].f1.y, gin[0].f1.x - gin[0].f0.x ) );

		position0.xy += normal * stick_width;
		v.position = mul( position0, mat_viewproj );
		gout.Append(v);
		position0.xy -= normal * stick_width * 2;
		v.position = mul( position0, mat_viewproj );
		gout.Append(v);
		v.position = mul( position1, mat_viewproj );
		gout.Append(v);
		gout.RestartStrip();
		
		v.position = mul( position1, mat_viewproj );
		gout.Append(v);
		position1.xy -= direction * arrow_size * 2;
		position1.xy -= normal * arrow_size;
		v.position = mul( position1, mat_viewproj );
		gout.Append(v);
		position1.xy += normal * arrow_size * 2;
		v.position = mul( position1, mat_viewproj );
		gout.Append(v);
	}
}

[maxvertexcount(10)]
void GSFrame( point GSInputType gin[1], inout LineStream<PSInputType>gout )
{
	PSInputType v;
	v.color = gin[0].color;

	float4 position = float4( gin[0].f0, 1.0 );
	float2 scaling = gin[0].f1.xy;

	position.xy -= scaling * 0.5;
	v.position = mul( position, mat_viewproj );
	float4 point0 = v.position;
	gout.Append(v);
	position.x += scaling.x;
	v.position = mul( position, mat_viewproj );
	gout.Append(v);
	position.y += scaling.y;
	v.position = mul( position, mat_viewproj );
	gout.Append(v);
	position.x -= scaling.x;
	v.position = mul( position, mat_viewproj );
	gout.Append(v);
	v.position = point0;
	gout.Append(v);

}
////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PS(PSInputType pin) : SV_TARGET
{
	//return float4( 0.0, 1.0, 1.0, 1.0 );
    return pin.color;
}
