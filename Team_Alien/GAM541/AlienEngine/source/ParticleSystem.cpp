#include "particleSystem.h"
#include "particleEmitter.h"
#include "DebugDiagnostic.h"


namespace Framework
{

//================================================================
void ParticleSystem::RegisterEmitter( Emitter* emitter )
{
	HRESULT hr;
	D3D10_BUFFER_DESC vb_desc;

	int max_vertices = emitter->max_particles;

	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DEFAULT;
	vb_desc.ByteWidth = sizeof( ParticleVertex ) * max_vertices;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
	vb_desc.CPUAccessFlags = 0;
	vb_desc.MiscFlags = 0;

	hr = device->CreateBuffer( &vb_desc, NULL, &emitter->vb[0] );
	ErrorIf( FAILED(hr), "failed building vertex buffer for particles" );

	hr = device->CreateBuffer( &vb_desc, NULL, &emitter->vb[1] );
	ErrorIf( FAILED(hr), "failed building vertex buffer for particles" );
	emitters.push_back( emitter );

	emitter->cur_target_buffer = 0;

}
//================================================================
void ParticleSystem::RemoveEmitter( Emitter* emitter )
{
	emitters.erase( emitter );
}
//================================================================
void ParticleSystem::InitializeLauncherVB( int num_launchers )
{
	D3D10_BUFFER_DESC vb_desc;

	ZeroMemory( &vb_desc, sizeof(vb_desc) );
	vb_desc.Usage = D3D10_USAGE_DYNAMIC;
	vb_desc.ByteWidth = sizeof( ParticleVertex ) * num_launchers;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	vb_desc.MiscFlags = 0;

	device->CreateBuffer( &vb_desc, NULL, &vb_launcher );
}

}