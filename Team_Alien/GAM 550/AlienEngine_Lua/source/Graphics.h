#pragma once

#include "System.h"
#include "ObjectLinkedList.h"
#include "Vector3.h"
#include "LineDrawer.h"

namespace Framework
{

// forward declarations
class D3DClass;//@@
class LineDrawer;
class Model;
class CameraClass;
class MeshShaderClass;
class ModelComponent;
class ModelComponent2;
class LightShaderClass;
class LightClass;
class BitmapClass;
class AnimateShaderClass;
class ColorShaderClass;
class FrustumClass;
class VisualPlatform;
class MultiTextureShaderClass;
class LightMapShaderClass;
class AlphaMapShaderClass;
class BumpMapShaderClass;
class ModelClass;
class SpecMapShaderClass;
class RenderTextureClass;
class DebugWindowClass;
class FogShaderClass;
class ClipPlaneShaderClass;
class TransparentShaderClass;
class TextClass;
class ModelInfo;
class Model2Info;
class TextureArrayClass;
class RenderTextureBitmapClass;
class FadeShaderClass;

enum MultiTextureType;
enum BumpType;

class Graphics : public ISystem
{
public:
	friend class ModelComponent;
	friend class ModelComponent2;
	friend class LineDrawer;
	LineDrawer lineDrawer;
private:
	HWND hwnd;
	
	CameraClass* m_Camera;
	LightShaderClass* m_LightShader;
	LightClass* m_Light;
	AnimateShaderClass* m_AnimateShader;
	ColorShaderClass* m_ColorShader;
	BitmapClass* m_Bitmap;
	FrustumClass* m_Frustum;
	MultiTextureShaderClass* m_MultiTextureShader;
	LightMapShaderClass* m_LightMapShader;
	AlphaMapShaderClass* m_AlphaMapShader;
	BumpMapShaderClass* m_BumpMapShader;
	SpecMapShaderClass* m_SpecMapShader;
	RenderTextureClass* m_RenderTexture;
	DebugWindowClass* m_DebugWindow;
	FogShaderClass* m_FogShader;
	ClipPlaneShaderClass* m_ClipPlaneShader;
	TextClass* m_Text;
	TransparentShaderClass* m_TransparentShader;
	FadeShaderClass* m_FadeShader;
	RenderTextureBitmapClass* m_RenderTextureBitmap;

	float m_fadeInTime, m_accumulatedTime, m_fadePercentage;
	bool m_fadeDone;

	float fogColor, fogStart, fogEnd;
	float blendAmount;

	D3DXVECTOR4 clipPlane;
	D3DXMATRIX worldMatrix, viewMatrix, orthoMatrix, projectionMatrix;
	D3DXMATRIX I;

	bool WireFrame;
	
	bool RenderToTexture(float dt);
	bool RenderFadingScene(float dt);
	void RenderScene(float dt);

	typedef std::map<std::string, TextureArrayClass*> TextureMap;
	TextureMap	GlobalTextures;
	void LoadGlobalTexture( std::string filename1, char* filename2, char* filename3);
	TextureArrayClass* GetGlobalTexture(std::string);//Get a texture asset. Will return null if texture is not loaded
	typedef std::map<std::string, Model*> ModelMap;
	ModelMap	Models;
	typedef std::map<std::string,ModelClass*> ModelMap2;
	ModelMap2	Models2;
	//void LoadModelToMap(const std::string& filename, char* textureLoc);
	void LoadModelToMap(ModelInfo*);
	void LoadModelToMap2(Model2Info*);
	ModelClass* GetModel2(std::string name);
	ObjectLinkList<ModelComponent> ModelComponentList;//@@@
	ObjectLinkList<ModelComponent2> ModelComponent2List;
	
public:
	Model* GetModel(std::string name);
	D3DClass* m_D3D;
	CameraClass* UI_Camera;
	CameraClass* Text_Camera;

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
	void SetWindowProperties( HWND hwnd, int width, int height, bool FullScreen  );
	void initializeAntTweakBar();
	void Graphics::ScreenToWorldSpaceRay( Vector3& , Vector3&  );
	void Graphics::ScreenToWorldSpace( Vector3& out,float depth);
	void screenPosition( Vector3& position);
	void TurnOnDebugCamera();
	void TurnOffDebugCamera();
	const D3DXVECTOR3& GetCameraPosition() const;
	const D3DXVECTOR3& GetCameraLookAt() const;
	D3DXMATRIX GetViewProjMatrix();
};
extern Graphics* GRAPHICS;
}
