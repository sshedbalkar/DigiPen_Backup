#pragma once

namespace Framework
{

class D3DManager
{
private:
	int video_card_memory;
	char video_card_name[128];

	ID3D11Device* device;
	ID3D11DeviceContext* device_context;

public:
	D3DManager();
	~D3DManager();

	void Initialize();
	void Shutdown();

	ID3D11Device* GetDevice() { return device; }
	ID3D11DeviceContext* GetDeviceContext() { return device_context; }
private:
	// disable copy constructor&operator=
	D3DManager( D3DManager& ) {};
	void operator=(D3DManager& ) {};

	// initialize steps
	void GetVideoCardInfos();
	void CreateSwapChainAndDevice();
};

}