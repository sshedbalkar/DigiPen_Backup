////////////////////////////////////////////////////////////////////////////////
// Filename: animated.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix WorldMatrixArray[80];
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
	float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 BlendWeights : BLENDWEIGHT;
    uint4 BlendIndices : BLENDINDICES;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType AnimateVertexShader(VertexInputType input)
{
    PixelInputType output;
	float4 worldPosition;
	float3 Pos = float3(0,0,0);
    float3 Normal = float3(0,0,0);	

    //input.position.w = 1.0f;
   
   // Change the position vector to be 4 units for proper matrix calculations.
    float BlendWeightsArray[4] = (float[4])input.BlendWeights;
    unsigned int   IndexArray[4]        = (unsigned int[4])input.BlendIndices;
	    
	for (int iBone = 0; iBone < 4 ; iBone++)
    {       
        Pos += mul( input.position , WorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
        Normal += mul(input.normal , WorldMatrixArray[IndexArray[iBone]]) * BlendWeightsArray[iBone];
    }
	Normal = normalize(Normal);

	//Pos = (float3)input.position;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul( input.position, worldMatrix);
	output.position = mul( float4(Pos, 1), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
    
	// Calculate the normal vector against the world matrix only.
    output.normal = mul(Normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

	// Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	
    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);

    return output;
}