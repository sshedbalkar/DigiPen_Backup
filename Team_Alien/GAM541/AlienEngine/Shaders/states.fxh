SamplerState sample_point_clamp
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

SamplerState sample_point_wrap
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState sample_linear_wrap
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

SamplerState sample_linear_clamp
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};
BlendState add_diffuse
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = ZERO;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

BlendState alpha_blend
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = ADD;
	SrcBlendAlpha = ZERO;
	DestBlendAlpha = ZERO;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0F;
	RenderTargetWriteMask[1] = 0x00;
	RenderTargetWriteMask[2] = 0x00;
};

BlendState noblend
{
};
DepthStencilState depthdisable
{
	DepthEnable				= FALSE;
};
DepthStencilState depthenable
{
	DepthEnable				= TRUE;
};

//static const float2 screen = { 1.0f/1280.0f, 1.0f/720.0f };

#define EFFECT_DIFFUSE					1
#define EFFECT_CELSHADING				2
#define EFFECT_SPECULAR					4
#define EFFECT_OUTLINE					8
#define EFFECT_WATERMARKING				16
#define EFFECT_SHADOW					32
#define EFFECT_NONBLOOMING				64
#define EFFECT_HALFBLOOM				128

#define PT_LAUNCHER						1
#define PT_SHRINKING					2
#define PT_ROTATING						4
#define PT_RANDOMROTATION				8

#define INTENSITY_LEVEL					5

#define LASER_LAUNCHER					0
#define LASER_WAVE_LAUNCHER				1
#define LASER_REFLECTED_WAVE_LAUNCHER	2
#define LASER_BEAM						3
#define LASER_WAVE						4

#define CLOUDS_LAUNCHER					0
#define CLOUDS_BRUSH_APPLYING			1
#define CLOUDS_BRUSH_APPLIED			2
#define CLOUDS_BRUSH_INACTIVATED		3
#define CLOUDS_PARTICLE					4

inline float luma( float4 color )
{
	return dot(color.rgb, float3(0.299, 0.587, 0.114) );
}

inline float4 GetRandom( Texture1D rtex, int seed, int offset )	//0 ~ 1
{
	float index = ((float)(seed)) / 2130.0f + (float)offset / 1000.0f;
	float4 ret = rtex.SampleLevel( sample_point_wrap, index, 0 );
	return ret;
}
