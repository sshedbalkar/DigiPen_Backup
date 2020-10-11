struct VertexType
{
	float3 position : FLOAT0;
};

VertexType VS( VertexType vin )
{
	return vin;
}

[maxvertexcount(100)]
void GS( point VertexType gin[1], inout PointStream<VertexType>gout )
{
	VertexType v = gin[0];

	for ( int i = 100; i > 0; --i )
	//for ( int i = 1; i <=100; ++i )
	{
		v.position = float3( 1.0, 2.0, (float)i / 100.0 );
		gout.Append( v );
	}
}
