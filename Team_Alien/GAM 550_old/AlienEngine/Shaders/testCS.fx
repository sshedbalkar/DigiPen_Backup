RWStructuredBuffer<float> buffer : register(u0);

[numthreads( 1, 1, 1 )]
void CS( uint3 gid : SV_DispatchThreadID )
{
	buffer[gid.x] = 2;
}