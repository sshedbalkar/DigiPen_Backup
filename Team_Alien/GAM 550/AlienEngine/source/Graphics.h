#pragma once

#include "System.h"
#include "ObjectLinkedList.h"
#include "Vector3.h"
#include "LineDrawer.h"
#define _XM_NO_INTRINSICS_
#include <xnamath.h>
#include <map>


namespace Framework
{

// forward declarations
class D3DClass;//@@
class LineDrawer;
class ModelClass;
class CameraClass;
class MeshShaderClass;
class ModelComponent;
class LightClass;
class BitmapClass;
class AnimateShaderClass;
class ColorShaderClass;
class FrustumClass;
class VisualPlatform;
class LightMapShaderClass;
class AlphaMapShaderClass;
class BumpMapShaderClass;
class SpecMapShaderClass;
class RenderTextureClass;
class DebugWindowClass;
class FogShaderClass;
class ClipPlaneShaderClass;
class TextClass;
class ModelInfo;
class TextureArrayClass;
class RenderTextureBitmapClass;
class FadeShaderClass;
class TextureShaderClass;
class DepthShaderClass;
class ParticleSystemClass;
class LightComponent;
class Instancing;

class Blur;
class GlowMapShaderClass;
class GlowShaderClass;

class PostProcessor;
struct StructuredBuffer;
struct RWBuffer;
class Deferred;
class FXAA;
struct MenuOptions;

enum MultiTextureType;
enum BumpType;

const int SHADOWMAP_WIDTH = 128;
const int SHADOWMAP_HEIGHT = 128;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


struct Light
{
    D3DXVECTOR3 Position;
    D3DXVECTOR3 Color;
    float Falloff;
};

class Graphics : public ISystem
{
	friend class TweakBarManager;
public:
	friend class ModelComponent;
	friend class LineDrawer;
	LineDrawer lineDrawer;
	std::vector<Light> DeferredLights;
	void SetNumOfLights(int k){NumLights=k;}
	int GetNoOfLights(){return NumLights;}
	int GetMAXNoOfLights(){return MaxLights;}
	void CreateLightBuffer();

	bool AA;

	void SetGraphicsOptions( MenuOptions* opt );
	void RollCredits();

	void FreeGraphicsObjectsBeforeResize();
	void ChangeResolution(int w, int h);

	float GetBloomThreshold() { return BloomThreshold; }
    float GetBloomMagnitude() { return BloomMagnitude; }
    float GetBloomBlurSigma() { return BloomBlurSigma; }
	float GetAdaptationRate() { return AdaptationRate; }
	float GetExposureKeyValue() { return ExposureKeyValue; }
	float GetSpecularIntensity() { return SpecularIntensity; }
	float GetSpecularR() { return SpecularR; }
	float GetSpecularG() { return SpecularG; }
	float GetSpecularB() { return SpecularB; }
	float GetBalance() { return Balance; }
	float GetRoughness() { return Roughness; }
	D3DXVECTOR4 GetConstAmbient() { return ConstAmbient; }
	D3DXVECTOR4 GetSkyColor() { return SkyColor; }
	D3DXVECTOR4 GetGroundColor() { return GroundColor; }
	float GetHemiIntensity() { return HemiIntensity; }

	void SetBloomThreshold(float temp) { BloomThreshold = temp; }
    void SetBloomMagnitude(float temp) { BloomMagnitude = temp; }
    void SetBloomBlurSigma(float temp) { BloomBlurSigma = temp; }
    void SetAdaptationRate(float temp) { AdaptationRate = temp; }
    void SetExposureKeyValue(float temp) { ExposureKeyValue = temp; }
	void SetSpecularIntensity(float temp) { SpecularIntensity = temp; }
	void SetSpecularR(float temp) { SpecularR = temp; }
	void SetSpecularG(float temp) { SpecularG = temp; }
	void SetSpecularB(float temp) { SpecularB = temp; }
	void SetBalance(float temp) { Balance = temp; }
	void SetRoughness(float temp) { Roughness = temp; }
	void SetConstAmbient(D3DXVECTOR4 temp) { ConstAmbient = temp; }
	void SetSkyColor(D3DXVECTOR4 temp) { SkyColor = temp; }
	void SetGroundColor(D3DXVECTOR4 temp) { GroundColor = temp; }
	void SetHemiIntensity(float temp) { HemiIntensity = temp; }
	bool DetermingIfInsideAdjacentToHeroLevelBox(int obj);
	bool DetermingIfInsideSameLevelBox(int obj1, int obj2);

	void SetStartGame() {m_StartGame = true;}
	void SetStopGame() {m_StartGame = false;}
	void Pause() {m_PauseGame = true;}
	void Unpause() {m_PauseGame = false;}
	bool IsPaused() { return m_PauseGame; }
	bool IsStartGame() { return m_StartGame; }

	void DeferredRender(float dt);
	void GlowRender(float dt);
	void TransparentRender(float dt);
	void Bloom(float dt);
	void Render2D(float dt);
	void RenderUI(float dt);
	void ForwardRender(float dt);
	void FullScreenFilter();

	int renderCount;
	bool renderModel;
	
	D3DXMATRIX IdentityMatrix;

	RenderTextureClass* GetPreBlurRT() { return PreBlurRT; }
	RenderTextureClass* GetGlowMapRT() { return GlowMapRT; }
	RenderTextureClass* GetPlaceholderRT() { return PlaceholderRT; }
	RenderTextureClass* GetBackBufferRT() { return BackBufferRT; }
	GlowMapShaderClass* GetGlowMapShader() { return m_GlowMapShader; }
	GlowShaderClass* GetGlowShader() { return m_GlowShader; }
	TextureShaderClass* GetTextureShader() { return m_TextureShader; }
	D3DXVECTOR3 TimePauseColor;
private:
	HWND hwnd;
	
	RenderTextureClass* BackBufferRT;

	// Logo object
	BitmapClass* m_DigipenLogo;
	BitmapClass* m_TeamLogo;
	BitmapClass* m_GameLogo;

	// Credits screen
	BitmapClass* m_Credits;

	// Start bools
	bool m_StartDigipenLogo, m_StartTeamLogo, m_StartGameLogo, m_StartGame, m_StartMenu, m_DoMenu, m_RollCredits, m_PauseGame;
	
	// Time speed textures
	BitmapClass* m_1x;
	BitmapClass* m_2x;
	BitmapClass* m_4x;
	BitmapClass* m_8x;
	BitmapClass* m_16x;

	CameraClass* m_Camera;
	std::vector<LightClass> m_Lights;
	AnimateShaderClass* m_AnimateShader;
	ColorShaderClass* m_ColorShader;
	BitmapClass* m_Bitmap;
	FrustumClass* m_Frustum;
	LightMapShaderClass* m_LightMapShader;
	AlphaMapShaderClass* m_AlphaMapShader;
	BumpMapShaderClass* m_BumpMapShader;
	SpecMapShaderClass* m_SpecMapShader;
	RenderTextureClass* m_RenderShadowTexture;
	RenderTextureClass* m_RenderFadeTexture;
	FogShaderClass* m_FogShader;
	ClipPlaneShaderClass* m_ClipPlaneShader;
	TextClass* m_Text;
	FadeShaderClass* m_FadeShader;
	RenderTextureBitmapClass* m_RenderTextureBitmap;
	DepthShaderClass* m_DepthShader;
	ParticleSystemClass* m_ParticleSystem;
	Instancing* m_pInstancing;

	TextureShaderClass *m_TextureShader;
	
	//FXAA
	FXAA* FxaaShader;
	RenderTextureClass* FxaaRT;
	void CalcFXAA();

	void ParticleFX(float dt);

	// Full screen filters
	RenderTextureClass* LightMappedScene;
	TextureArrayClass* LightMapTAC;

	// Blur+Glow variables
	Blur* BlurShader;
	RenderTextureClass* PreBlurRT;
	RenderTextureClass* GlowMapRT;
	RenderTextureClass* PlaceholderRT;
	GlowMapShaderClass* m_GlowMapShader;
	GlowShaderClass* m_GlowShader;

	// Debug Windows
	DebugWindowClass* m_DebugWindow1;
	DebugWindowClass* m_DebugWindow2;
	DebugWindowClass* m_DebugWindow3;
	DebugWindowClass* m_DebugWindow4;
	DebugWindowClass* m_DebugWindow5;
	DebugWindowClass* m_DebugWindow6;

	//DEFERRED VARIABLES
	int LightTileSize;
	int numTilesX;
    int numTilesY;
    int numElements;
    DXGI_FORMAT format;
    int stride;	
	int NumSamples;
    int NumLights;
	int MaxLights;
	int dtWidth;
    int dtHeight;
	RenderTextureClass* colorTarget;
	RenderTextureClass* deferredOutputTarget;
	RenderTextureClass* normalsTarget;
	RenderTextureClass* diffuseAlbedoTarget;
    RenderTextureClass* specularAlbedoTarget;
    RenderTextureClass* lightingTarget;
	RenderTextureClass* depthBuffer;
	PostProcessor* pp;
	StructuredBuffer* lightsBuffer;
	Deferred* Def;
	float BloomThreshold;
    float BloomMagnitude;
    float BloomBlurSigma;
    float AdaptationRate;
    float ExposureKeyValue;
	float SpecularIntensity;
	float SpecularR;
	float SpecularG;
	float SpecularB;
	float Balance;
	float Roughness;
	D3DXVECTOR4 ConstAmbient;
	D3DXVECTOR4 SkyColor;
	D3DXVECTOR4 GroundColor;
	float HemiIntensity;

	

	// Deferred Functions
	void LoadContent();
	
	void ComputeLightTiles();

	float m_fadeInTime, m_accumulatedTime, m_fadePercentage;
	float fogColor, fogStart, fogEnd;

	D3DXVECTOR4 clipPlane;
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix, projectionMatrix;

	bool WireFrame;
	bool DW1;
	bool DW2;
	bool DW3;
	bool DW4;
	bool DW5;
	bool DW6;
	bool DebugDraw;
	int FrameRateCacAccuCounter;
	float FrameRateCacAccuTimer;
	float FrameRate;

	bool RenderFadeTexture(float dt);
	bool RenderShadowTexture(float dt);
	void RenderScene(float dt);
	bool RenderCubeMapTexture(float dt, int LightIndex);

	typedef std::map<std::string, TextureArrayClass*> TextureMap;
	TextureMap	textures;
	void LoadTexture(const std::string&, char* filename1, char* filename2, char* filename3, MultiTextureType _mtt, bool Extendable);
	TextureArrayClass* GetTexture(std::string);//Get a texture asset. Will return null if texture is not loaded
	typedef std::map<std::string, ModelClass*> ModelMap;
	ModelMap	Models;
	void LoadModelToMap(const std::string& );
	void LoadModelToMap2(const std::string& );
	void LoadModelToMapInfo(ModelInfo*);
	void LoadModelToMap2Info(ModelInfo*);
	ModelClass* GetModel(std::string name);
	ObjectLinkList<ModelComponent> ModelComponentList;//@@@
	//for instancing
	unsigned int maxObjectCount_thisLevel;
	ModelComponent** pCubeModelThisFrameArray;
public:
	unsigned int GetmaxObjectCount_thisLevel() { return maxObjectCount_thisLevel; }
	inline void maxObjectCount_thisLevelPlusOne() { ++maxObjectCount_thisLevel; };
	void CreateInstancingArray();
	Deferred* GetDeferred();

	D3DClass* m_D3D;

	int screen_width;
	int screen_height;

	Graphics();
	~Graphics();
	virtual void Update( float dt );
	virtual void Initialize();
	virtual void Free();
	virtual void SendMessage( Message* m );
	virtual void Unload();

	virtual std::string GetName() { return "Graphics"; }
	void SetWindowProperties( HWND hwnd, int width, int height );
	void initializeAntTweakBar();
	void ScreenToWorldSpaceRay( Vector3& , Vector3&  );
	void ScreenToWorldSpace( Vector3& out,float depth);
	void screenPosition( Vector3& position);
	void TurnOnDebugCamera();
	void TurnOffDebugCamera();
	bool Graphics::IsDebugCameraON();
	void PushLightToList(LightClass);
	void DestroyAllLights();//for the editor
	void SetDebugDraw(bool b){DebugDraw =b;}
	void AddTwoPointsToLineDrawer(Vector3 , Vector3 , D3DXVECTOR4 a=D3DXVECTOR4(1,0,0,1));

	// Computes a compute shader dispatch size given a thread group size, and number of elements to process
	UINT32 DispatchSize(UINT32 tgSize, UINT32 numElements)
	{
		UINT32 dispatchSize = numElements / tgSize;
		dispatchSize += numElements % tgSize > 0 ? 1 : 0;
		return dispatchSize;
	}

	D3DXVECTOR3 GetCameraPosition() const;
	D3DXVECTOR3 GetCameraLookAt() const;
	D3DXMATRIX GetViewProjMatrix();
	TextClass* GetTextObj();
	D3DClass* GetD3D();
	HWND GetH() {return hwnd;}
	CameraClass* GetCamera() {return m_Camera;}
	ParticleSystemClass* GetParticleSystemClass();
};
extern Graphics* GRAPHICS;
}

