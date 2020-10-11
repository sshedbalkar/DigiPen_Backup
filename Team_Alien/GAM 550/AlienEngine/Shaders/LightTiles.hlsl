//=================================================================================================
//
//	Light Indexed Deferred Sample
//  by MJP
//  http://mynameismjp.wordpress.com/
//
//  All code and content licensed under Microsoft Public License (Ms-PL)
//
//=================================================================================================

//=================================================================================================
// Includes
//=================================================================================================
#include "Lighting.hlsl"

static const uint LightTileSize = 16;

struct Light
{
    float3 Position;
    float3 Color;
    float Falloff;
};

//=================================================================================================
// Constant buffers
//=================================================================================================
cbuffer CSConstants : register(cb0)
{
    float4x4 View;
	float4x4 Projection;
    float4x4 InvViewProjection;
    float3 CameraPosWS;
	float2 CameraClipPlanes;
    uint2 NumTiles;
    uint2 DisplaySize;
	bool TimePaused;
	float3 TimePauseColor;
}

//=================================================================================================
// Constants
//=================================================================================================
static const uint ThreadGroupSize = LightTileSize * LightTileSize;

//=================================================================================================
// Resources
//=================================================================================================

// Inputs
StructuredBuffer<Light> Lights : register(t0);

Texture2D<float> Depth : register(t1);
Texture2D<float4> Normals : register(t2);
Texture2D<float4> DiffuseAlbedo : register(t3);
Texture2D<float4> SpecularAlbedo : register(t4);
Texture2D<float4> Lighting : register(t5);

// Outputs
RWTexture2D<float4> OutputTexture : register(u0);

// Shared memory
groupshared uint TileMinZ;
groupshared uint TileMaxZ;

// Light list for the tile
groupshared uint TileLightList[MaxLights];
groupshared uint NumTileLights;

// ------------------------------------------------------------------------------------------------
// Converts a z-buffer depth to linear depth
// ------------------------------------------------------------------------------------------------
float LinearDepth(in float zw)
{
    return Projection._43 / (zw - Projection._33);
}

// ------------------------------------------------------------------------------------------------
// Calculates position from a depth value + pixel coordinate
// ------------------------------------------------------------------------------------------------
float3 PositionFromDepth(in float zw, in uint2 pixelCoord)
{
    float2 cpos = (pixelCoord + 0.5f) / DisplaySize;
    cpos *= 2.0f;
    cpos -= 1.0f;
    cpos.y *= -1.0f;
    float4 positionWS = mul(float4(cpos, zw, 1.0f), InvViewProjection);
    return positionWS.xyz / positionWS.w;
}

//=================================================================================================
// Light tile assignment for light indexed deferred, or full tile-based deferred lighting
//=================================================================================================
[numthreads(LightTileSize, LightTileSize, 1)]
void LightTiles(uint3 GroupID : SV_GroupID, uint3 GroupThreadID : SV_GroupThreadID)
{
	uint2 pixelCoord = GroupID.xy * uint2(LightTileSize, LightTileSize) + GroupThreadID.xy;

	const uint groupThreadIdx = GroupThreadID.y * LightTileSize + GroupThreadID.x;

	// Work out Z bounds for our samples
    float minZSample = CameraClipPlanes.y;
    float maxZSample = CameraClipPlanes.x;

    // Calculate view-space Z from Z/W depth	
    float zw = Depth[pixelCoord];

	float linearZ = LinearDepth(zw);
    float3 positionWS = PositionFromDepth(zw, pixelCoord);
	    
    minZSample = min(minZSample, linearZ);
    maxZSample = max(maxZSample, linearZ);

    // Initialize shared memory light list and Z bounds
    if(groupThreadIdx == 0)
	{
        NumTileLights = 0;
		
        TileMinZ = 0x7F7FFFFF;      // Max float
        TileMaxZ = 0;
    }

    GroupMemoryBarrierWithGroupSync();

    if(maxZSample >= minZSample)
	{
        InterlockedMin(TileMinZ, asuint(minZSample));
        InterlockedMax(TileMaxZ, asuint(maxZSample));
    }

    GroupMemoryBarrierWithGroupSync();

    float minTileZ = asfloat(TileMinZ);
    float maxTileZ = asfloat(TileMaxZ);

    // Work out scale/bias from [0, 1]
    float2 tileScale = float2(DisplaySize.xy) * rcp(2.0f * float2(LightTileSize, LightTileSize));
    float2 tileBias = tileScale - float2(GroupID.xy);

    // Now work out composite projection matrix
    // Relevant matrix columns for this tile frusta
    float4 c1 = float4(Projection._11 * tileScale.x, 0.0f, tileBias.x, 0.0f);
    float4 c2 = float4(0.0f, -Projection._22 * tileScale.y, tileBias.y, 0.0f);
    float4 c4 = float4(0.0f, 0.0f, 1.0f, 0.0f);

    // Derive frustum planes
    float4 frustumPlanes[6];

    // Sides
    frustumPlanes[0] = c4 - c1;
    frustumPlanes[1] = c4 + c1;
    frustumPlanes[2] = c4 - c2;
    frustumPlanes[3] = c4 + c2;

    // Near/far
    frustumPlanes[4] = float4(0.0f, 0.0f,  1.0f, -minTileZ);
    frustumPlanes[5] = float4(0.0f, 0.0f, -1.0f,  maxTileZ);

    // Normalize frustum planes (near/far already normalized)
    [unroll]
	for (uint i = 0; i < 4; ++i)
        frustumPlanes[i] *= rcp(length(frustumPlanes[i].xyz));

    // Cull lights for this tile
    [unroll]
    for(uint lightIndex = groupThreadIdx; lightIndex < MaxLights; lightIndex += ThreadGroupSize)
	{
		float3 lightPosition = mul(float4(Lights[lightIndex].Position, 1.0f), View).xyz;
		float cutoffRadius = Lights[lightIndex].Falloff;

        // Cull: point light sphere vs tile frustum
        bool inFrustum = true;
        [unroll]
		for(uint i = 0; i < 6; ++i)
		{
            float d = dot(frustumPlanes[i], float4(lightPosition, 1.0f));
            inFrustum = inFrustum && (d >= -cutoffRadius);
        }

        [branch]
		if(inFrustum)
		{
            uint listIndex;
            InterlockedAdd(NumTileLights, 1, listIndex);
            TileLightList[listIndex] = lightIndex;
        }
    }

    GroupMemoryBarrierWithGroupSync();
	    
    float3 normalWS = Normals[pixelCoord].xyz * 2.0f - 1.0f;
    float3 diffuseAlbedo = DiffuseAlbedo[pixelCoord].xyz;
    float3 specularAlbedo = SpecularAlbedo[pixelCoord].xyz;
    float roughness = SpecularAlbedo[pixelCoord].w;
    float3 lighting = Lighting[pixelCoord].xyz;
    

    // Light the pixel
    for(uint tLightIdx = 0; tLightIdx < NumTileLights; ++tLightIdx)
    {
        uint lIdx = TileLightList[tLightIdx];
        Light light = Lights[lIdx];

        lighting += CalcPointLight(normalWS, light.Color, diffuseAlbedo, specularAlbedo,
                                    roughness, positionWS.xyz, light.Position, light.Falloff, CameraPosWS);
    }

    
	if(TimePaused)
	{
		//lighting.xyz = (lighting.x+lighting.y+lighting.z)/3.0f; // Regular pixel average
		lighting.xyz = (0.21f*lighting.x)+(0.71f*lighting.y)+(0.07f*lighting.z); // Luminosity (better looking pixel average)
		lighting.xyz = float3(lighting.x*TimePauseColor.x, lighting.y*TimePauseColor.y, lighting.z*TimePauseColor.z); // Multiply with desired color
	}

    OutputTexture[pixelCoord] = float4(lighting, 1.0f);
}