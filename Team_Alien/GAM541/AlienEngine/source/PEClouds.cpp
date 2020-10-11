#include "PEClouds.h"
#include "particleEmitter.h"
#include "shader.h"
#include "Graphics.h"


namespace Framework
{

//=======================================================
PEClouds::PEClouds( ID3D10Device* _device ) : ParticleSystem(_device)
{
}
//=======================================================
PEClouds::~PEClouds()
{
	Free();
}
//=======================================================
void PEClouds::Initialize()
{
	advance_particles = new Shader;
	advance_particles->Initialize( device, "shaders\\advanceParticlesClouds.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1|VERTEX_ELEMENT_POSITION2|VERTEX_ELEMENT_PER_VERTEX_FLAG );
	screen_range_ptr = advance_particles->GetVariableByName("screen_range")->AsVector();
	global_time_ptr = advance_particles->GetVariableByName("global_time")->AsScalar();
	elapsed_time_ptr = advance_particles->GetVariableByName("elapsed_time")->AsScalar();
//	deactivating_ptr = advance_particles->GetVariableByName("deactivating")->AsScalar();
	random_texture_ptr = advance_particles->GetVariableByName("random_texture")->AsShaderResource();

	render_particles = new Shader;
	render_particles->Initialize( device, "shaders\\particleShaderClouds.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1|VERTEX_ELEMENT_POSITION2|VERTEX_ELEMENT_PER_VERTEX_FLAG );
//	color_ptr = render_particles->GetVariableByName("color")->AsVector();
//	flag_ptr = render_particles->GetVariableByName("flag")->AsScalar();	
//	mat_world_ptr = render_particles->GetVariableByName("mat_world")->AsMatrix();

	InitializeLauncherVB( 1 );

	random_texture_ptr->SetResource( GRAPHICS->GetRandomTexture() );
}
//=======================================================
void PEClouds::Update( float timeslice )
{
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	unsigned int stride = sizeof( ParticleVertex );
	unsigned int offset = 0;
	ID3D10Buffer* target;
	ID3D10Buffer* source;
	float screen_range[2];

	GRAPHICS->GetScreenRange( screen_range, 100.0f );
//	printf("%0.3f %0.3f\n", screen_range[0], screen_range[1] );
	/// set per frame constants
	global_time_ptr->SetInt((unsigned int)timeGetTime());
	elapsed_time_ptr->SetFloat( timeslice );
	screen_range_ptr->SetFloatVector( screen_range );
	

	/// loop through all emitters
	EmitterListType::iterator it;
	for ( it = emitters.begin(); it != emitters.end(); ++it )
	{
//		Vec2 half_scale = Vec2( it->parent->transform->Scale.x * 0.5f, it->parent->transform->Scale.y * 0.5f );
//		half_scale_ptr->SetFloatVector( half_scale );
//		deactivating_ptr->SetBool( it->deactivating );

		it->Update( timeslice );

		/// advance one frame
		/// if the emitter is new, set up the launcher
		advance_particles->Apply( device );	
		if ( it->initializing )
		{
			ParticleVertex* launcher;
			vb_launcher->Map( D3D10_MAP_WRITE_DISCARD, 0, (void**)&launcher );
			launcher[0].v0 = it->parent->transform->Position;		// position
			launcher[0].v1 = Vec3( 99999.0f, -99999.0f, 0.0f );	// min_x, max_x
			launcher[0].v2 = Vec3( 0.0f, 0.0f, 0.0f );		// timer
			launcher[0].type = 0;
			vb_launcher->Unmap();
			target = it->vb[it->cur_target_buffer];
			device->IASetVertexBuffers( 0, 1, &vb_launcher, &stride, &offset );
			device->SOSetTargets( 1, &target, &offset );
			device->Draw( 1, 0 );
			it->initializing = false;
		}
		else
		{
			source = it->vb[it->cur_target_buffer];
			target = it->vb[1 - it->cur_target_buffer];
			device->IASetVertexBuffers( 0, 1, &source, &stride, &offset );
			device->SOSetTargets( 1, &target, &offset );
			device->DrawAuto();
			it->cur_target_buffer = 1 - it->cur_target_buffer;
		}
	}//end for
	target = NULL;
	device->SOSetTargets( 1, &target, &offset );

}
//=======================================================
void PEClouds::Draw( Mat4& mat_view, Mat4& mat_proj )
{
	render_particles->SetTransformationConstants( device, mat_view, mat_proj );
	
	EmitterListType::iterator it;
	for ( it = emitters.begin(); it != emitters.end(); ++it )
	{
		if ( !it->activated ) continue;
		
		//flag_ptr->SetInt( it->flag );
		//color_ptr->SetFloatVector( it->color );
		render_particles->Apply( device );
		uint stride = sizeof(ParticleVertex);
		uint offset = 0;
		device->IASetVertexBuffers( 0, 1, &it->vb[it->cur_target_buffer], &stride, &offset );
		device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
		device->DrawAuto();
	}


}
//=======================================================
void PEClouds::Free()
{
	S_DELETE( advance_particles );
	S_DELETE( render_particles );
	S_RELEASE( vb_launcher );
}

}