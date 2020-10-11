//#include "Precompiled.h"
#include <D3D11.h>
#include <string>
#include "DebugDiagnostic.h"
#include "d3dmanager.h"

namespace Framework
{



//=============================================
D3DManager::D3DManager()
{
}
//=============================================
D3DManager::~D3DManager()
{
	this->Shutdown();
}
//=============================================
void D3DManager::Shutdown()
{
	if ( device_context ) { device_context->Release(); device_context = NULL; }
	if ( device ) { device->Release(); device = NULL; }
}
//=============================================
void D3DManager::Initialize()
{
	
	/*************************************
	  getting video card information
	*************************************/
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapter_output;
	unsigned int num_modes, i, numerator, denominator, string_length;
	DXGI_MODE_DESC* display_mode_list;
	DXGI_ADAPTER_DESC adapter_desc;
	int error;

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	ErrorIf( FAILED(result),"failed creating dxgi factory\n" );

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	ErrorIf( FAILED(result), "failed creating an adapter for video card.\n" );

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapter_output);
	ErrorIf( FAILED(result), "failed enumerating the adapter outputs.\n" );

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, NULL);
	ErrorIf( FAILED(result), "failed getting number of modes for the adapter.\n" );

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	display_mode_list = new DXGI_MODE_DESC[num_modes];
	ErrorIf( !display_mode_list );

	// Now fill the display mode list structures.
	result = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, display_mode_list);
	ErrorIf( FAILED(result), "failed loading dislay modes.\n" );

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for( i = 0; i < num_modes; ++i )
	{
//		printf("		%d x %d\n", display_mode_list[i].Width, display_mode_list[i].Height );
		//numerator = displayModeList[i].RefreshRate.Numerator;
		//denominator = displayModeList[i].RefreshRate.Denominator;
	}

	// Get the adapter (video card) description.
	result = adapter->GetDesc( &adapter_desc );
	ErrorIf( FAILED(result), "failed getting description info from the adapter.\n" );

	// Store the dedicated video card memory in megabytes.
	video_card_memory = (int)(adapter_desc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&string_length, video_card_name, 128, adapter_desc.Description, 128);
	ErrorIf( error != 0, "failed getting name of the video card.\n" );

//	printf( "		%s\n", video_card_name );

	// Release the display mode list.
	delete [] display_mode_list;
	display_mode_list = 0;

	// Release the adapter output.
	adapter_output->Release();
	adapter_output = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	/*************************************
	  creating swap chain and device
	*************************************/
	D3D_FEATURE_LEVEL feature_level[2];
	D3D_FEATURE_LEVEL supportted_feature_level;


	feature_level[0] = D3D_FEATURE_LEVEL_11_0;
	feature_level[1] = D3D_FEATURE_LEVEL_10_1;
	D3D11CreateDevice( NULL, 
					   D3D_DRIVER_TYPE_HARDWARE,
					   NULL,
					   0,
					   feature_level,
					   2,
					   D3D11_SDK_VERSION,
					   &device,
					   &supportted_feature_level,
					   &device_context );

	
	//printf( "%d\n", supportted_feature_level == D3D_FEATURE_LEVEL_10_1 );


}


}
