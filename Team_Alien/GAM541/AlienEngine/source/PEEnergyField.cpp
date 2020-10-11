#include "PEEnergyField.h"
#include "particleEmitter.h"
#include "shader.h"
#include "Graphics.h"

#define MAX_LAUNCHERS		120

namespace Framework
{

//=======================================================
PEEnergyField::PEEnergyField( ID3D10Device* _device ) : ParticleSystem(_device)
{
}
//=======================================================
PEEnergyField::~PEEnergyField()
{
	Free();
}
//=======================================================
void PEEnergyField::Initialize()
{
	advance_particles = new Shader;
	advance_particles->Initialize( device, "shaders\\advanceParticlesEnergyField.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1|VERTEX_ELEMENT_POSITION2|VERTEX_ELEMENT_PER_VERTEX_FLAG );
	half_scale_ptr = advance_particles->GetVariableByName("half_scale")->AsVector();
	global_time_ptr = advance_particles->GetVariableByName("global_time")->AsScalar();
	elapsed_time_ptr = advance_particles->GetVariableByName("elapsed_time")->AsScalar();
	deactivating_ptr = advance_particles->GetVariableByName("deactivating")->AsScalar();
	random_texture_ptr = advance_particles->GetVariableByName("random_texture")->AsShaderResource();

	render_particles = new Shader;
	render_particles->Initialize( device, "shaders\\particleShaderEnergyField.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1|VERTEX_ELEMENT_POSITION2|VERTEX_ELEMENT_PER_VERTEX_FLAG );
	color_ptr = render_particles->GetVariableByName("color")->AsVector();
	mat_world_ptr = render_particles->GetVariableByName("mat_world")->AsMatrix();

	InitializeLauncherVB( MAX_LAUNCHERS );

	random_texture_ptr->SetResource( GRAPHICS->GetRandomTexture() );
}
//=======================================================
void PEEnergyField::Update( float timeslice )
{
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	unsigned int stride = sizeof( ParticleVertex );
	unsigned int offset = 0;
	ID3D10Buffer* target;
	ID3D10Buffer* source;

	/// set per frame constants
	global_time_ptr->SetInt((unsigned int)timeGetTime());
	elapsed_time_ptr->SetFloat( timeslice );

	/// loop through all emitters
	EmitterListType::iterator it;
	for ( it = emitters.begin(); it != emitters.end(); ++it )
	{
		Vec2 half_scale = Vec2( it->parent->transform->Scale.x * 0.5f, it->parent->transform->Scale.y * 0.5f );
		half_scale_ptr->SetFloatVector( half_scale );
		deactivating_ptr->SetBool( it->deactivating );

		it->Update( timeslice );

		/// advance one frame
		/// if the emitter is new, set up the launcher
		advance_particles->Apply( device );	
		if ( it->initializing )
		{
			float area = it->parent->transform->GetArea();
			int num_launchers = (int)((float)(MAX_LAUNCHERS) * area / 3.0f);
			num_launchers = num_launchers > MAX_LAUNCHERS ? MAX_LAUNCHERS : num_launchers;
			ParticleVertex* launcher;
			vb_launcher->Map( D3D10_MAP_WRITE_DISCARD, 0, (void**)&launcher );
			for ( int i = 0; i < num_launchers; ++i )
			{
				launcher[i].type = 0;
				launcher[i].v0.z = Rand( 0.0f, 1000.0f );//(float) (( i ) * 4);
			}
			vb_launcher->Unmap();
			target = it->vb[it->cur_target_buffer];
			device->IASetVertexBuffers( 0, 1, &vb_launcher, &stride, &offset );
			device->SOSetTargets( 1, &target, &offset );
			device->Draw( num_launchers, 0 );
			it->initializing = false;
		}
		else
		{
			source = it->vb[it->cur_target_buffer];
			target = it->vb[1 - it->cur_target_buffer];
			device->IASetVertexBuffers( 0, 1, &source, &stride, &offset );
			device->SOSetTargets( 1, &target, &offset );
//	advance_particles->Apply( device );	
			device->DrawAuto();
			it->cur_target_buffer = 1 - it->cur_target_buffer;
		}
	}//end for
	target = NULL;
	device->SOSetTargets( 1, &target, &offset );
}
//=======================================================
void PEEnergyField::Draw( Mat4& mat_view, Mat4& mat_proj )
{
	Mat4 mat_world;
	render_particles->SetTransformationConstants( device, mat_view, mat_proj );
	
	EmitterListType::iterator it;
	for ( it = emitters.begin(); it != emitters.end(); ++it )
	{
		if ( !it->activated ) continue;
		
		it->parent->transform->BuildMatrixNoScale( mat_world );
		mat_world_ptr->SetMatrix( (float*)&mat_world );
		color_ptr->SetFloatVector( it->color );
		render_particles->Apply( device );
		uint stride = sizeof(ParticleVertex);
		uint offset = 0;
		device->IASetVertexBuffers( 0, 1, &it->vb[it->cur_target_buffer], &stride, &offset );
		device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
		device->DrawAuto();
	}

}
//=======================================================
void PEEnergyField::Free()
{
	S_DELETE( advance_particles );
	S_DELETE( render_particles );
	S_RELEASE( vb_launcher );
}

}