cbuffer PerFrameBuffer : register(b0)
{
	matrix mat_viewproj;
};

struct VertexType
{
	float3 position : FLOAT0;
};

struct PSInputType
{
	float4 positionCS : SV_POSITION;
};

VertexType VS( VertexType vin )
{
	return vin;
}

[maxvertexcount(4)]
void GS( point VertexType gin[1], inout TriangleStream<PSInputType> gout )
{
	PSInputType v;
	float3 position;
	position = gin[0].position;
	//float width = 1.0;
	position.xy -= float2( 2.0, 2.0 );
	v.positionCS = mul( float4( position, 1.0 ), mat_viewproj );
	gout.Append(v);
	
	position.y += 4.0;
	v.positionCS = mul( float4( position, 1.0 ), mat_viewproj );
	gout.Append(v);

	position.x += 4.0;
	position.y -= 4.0;
	v.positionCS = mul( float4( position, 1.0 ), mat_viewproj );
	gout.Append(v);

	position.y += 4.0;
	v.positionCS = mul( float4( position, 1.0 ), mat_viewproj );
	gout.Append(v);
}

float4 PS( PSInputType pin ) : SV_Target0
{
	return float4( 0.0, 0.0, 0.0, 1.0 );
}