#pragma once

#include "System.h"
#include "ObjectLinkedList.h"


namespace Framework
{

// forward declarations
class D3DClass;//@@
class Model;
class CameraClass;
class MeshShaderClass;
class ModelComponent;
class LightShaderClass;
class LightClass;
class BitmapClass;
class AnimateShaderClass;
class ColorShaderClass;

class Graphics : public ISystem
{
public:
	friend class ModelComponent;//@@@
private:
	HWND hwnd;
	int screen_width;
	int screen_height;

	D3DClass* m_D3D;//@@
	CameraClass* m_Camera;
	LightShaderClass* m_LightShader;
	LightClass* m_Light;
	AnimateShaderClass* m_AnimateShader;
	ColorShaderClass* m_ColorShader;
	BitmapClass* m_Bitmap;
	class VisualPlatform;

	typedef std::map<std::string,Model*> ModelMap;
	ModelMap	Models;
	void LoadModelToMap(const std::string& filename, char* textureLoc);
	Model* GetModel(std::string name);

	ObjectLinkList<ModelComponent> ModelComponentList;//@@@

	void DrawLine(D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR4);
public:
	Graphics();
	~Graphics();
	virtual void Update( float dt );
	virtual void Initialize();
	virtual void Free();
	virtual void SendMessage( Message* m );
	virtual void Unload();

	virtual std::string GetName() { return "Graphics"; }
	void SetWindowProperties( HWND hwnd, int width, int height, bool FullScreen  );
};
extern Graphics* GRAPHICS;
}
