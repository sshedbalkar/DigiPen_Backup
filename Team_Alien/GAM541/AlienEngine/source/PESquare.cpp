#include "PESquare.h"
#include "particleEmitter.h"
#include "shader.h"
#include "Graphics.h"

namespace Framework
{

//=======================================================
PESquare::PESquare( ID3D10Device* _device ) : ParticleSystem(_device)
{
}
//=======================================================
PESquare::~PESquare()
{
	Free();
}
//=======================================================
void PESquare::Initialize()
{
	advance_particles = new Shader;
	advance_particles->Initialize( device, "shaders\\advanceParticlesSquare.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1|VERTEX_ELEMENT_POSITION2|VERTEX_ELEMENT_PER_VERTEX_FLAG );
	initial_position_bottomleft_ptr = advance_particles->GetVariableByName("initial_position_bottomleft")->AsVector();
	initial_position_topright_ptr = advance_particles->GetVariableByName("initial_position_topright")->AsVector();
	particle_radius_ptr = advance_particles->GetVariableByName("particle_radius")->AsScalar();
	initial_velocity_ptr = advance_particles->GetVariableByName("initial_velocity")->AsVector();
	velocity_deviation_ptr = advance_particles->GetVariableByName("velocity_deviation")->AsVector();
	life_span_ptr = advance_particles->GetVariableByName("life_span")->AsScalar();
	life_span_deviation_ptr = advance_particles->GetVariableByName("life_span_deviation")->AsScalar();
	global_time_ptr = advance_particles->GetVariableByName("global_time")->AsScalar();
	elapsed_time_ptr = advance_particles->GetVariableByName("elapsed_time")->AsScalar();
	acceleration_ptr = advance_particles->GetVariableByName("acceleration")->AsVector();
	deactivating_ptr = advance_particles->GetVariableByName("deactivating")->AsScalar();
	random_texture_ptr = advance_particles->GetVariableByName("random_texture")->AsShaderResource();
	mat_world_ptr = advance_particles->GetVariableByName("mat_world")->AsMatrix();

	render_particles = new Shader;
	render_particles->Initialize( device, "shaders\\particleShaderSquare.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1|VERTEX_ELEMENT_POSITION2|VERTEX_ELEMENT_PER_VERTEX_FLAG );
	color_ptr = render_particles->GetVariableByName("color")->AsVector();
	flag_ptr = render_particles->GetVariableByName("flag")->AsScalar();	

	InitializeLauncherVB(1);

	random_texture_ptr->SetResource( GRAPHICS->GetRandomTexture() );
}
//=======================================================
void PESquare::Update( float timeslice )
{
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	unsigned int stride = sizeof( ParticleVertex );
	unsigned int offset = 0;
	ID3D10Buffer* target;
	ID3D10Buffer* source;
	Mat4 mat_world;

	/// set per frame constants
	global_time_ptr->SetInt((unsigned int)timeGetTime());
	elapsed_time_ptr->SetFloat( timeslice );

	/// loop through all emitters
	EmitterListType::iterator it;
	for ( it = emitters.begin(); it != emitters.end(); ++it )
	{
		/// emitter deactivating 
		if ( it->deactivating )
		{
			it->timer -= timeslice;
			if ( it->timer < 0.0f ) it->activated = false;
		}
		/// emitter inactivated
		if ( !it->activated ) continue;

		initial_position_bottomleft_ptr->SetFloatVector( it->offset_bottomleft );
		initial_position_topright_ptr->SetFloatVector( it->offset_topright );
		particle_radius_ptr->SetFloat( it->radius );
		life_span_ptr->SetFloat( it->life_span );
		life_span_deviation_ptr->SetFloat( it->life_span_deviation );
		initial_velocity_ptr->SetFloatVector( it->initial_velocity );
		velocity_deviation_ptr->SetFloatVector( it->velocity_deviation );
		deactivating_ptr->SetBool( it->deactivating );
		acceleration_ptr->SetFloatVector( it->acceleration );

		it->Update( timeslice );
		it->GetWorldTransformation( mat_world );
		mat_world_ptr->SetMatrix( (float*)&mat_world );

		/// advance one frame
		/// if the emitter is new, set up the launcher
		advance_particles->Apply( device );	
		if ( it->initializing )
		{
			ParticleVertex* launcher;
			vb_launcher->Map( D3D10_MAP_WRITE_DISCARD, 0, (void**)&launcher );
			launcher[0].type = PT_LAUNCHER | it->type;
			launcher[0].v2.x = it->interval;	/// first element of v2_timer will store the interval
			launcher[0].v2.z = 0.0f;				/// third element is the timer
			launcher[0].v1 = Vec3( it->particles_per_millisecond, 0.0f, 0.0f );
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
//	advance_particles->Apply( device );	
			device->DrawAuto();
			it->cur_target_buffer = 1 - it->cur_target_buffer;
		}
	}//end for
	target = NULL;
	device->SOSetTargets( 1, &target, &offset );
}
//=======================================================
void PESquare::Draw( Mat4& mat_view, Mat4& mat_proj )
{
	render_particles->SetTransformationConstants( device, mat_view, mat_proj );
	
	EmitterListType::iterator it;
	for ( it = emitters.begin(); it != emitters.end(); ++it )
	{
		if ( !it->activated ) continue;
		//d3dmanager->SetDepthEnable( true );
		//if ( it->blend_method == BLEND_ADD_DIFFUSE )
		//	d3dmanager->SetBlendAddDiffuse();
		//else if ( it->blend_method == BLEND_SUB_DIFFUSE )
		//	d3dmanager->SetBlendSubDiffuse();
		//else 
		//{
		//	d3dmanager->SetBlendDisable();
		//	d3dmanager->SetDepthEnable( true );
		//}
		
		color_ptr->SetFloatVector( it->color );
		flag_ptr->SetInt( it->flag );
		render_particles->Apply( device );
		uint stride = sizeof(ParticleVertex);
		uint offset = 0;
		device->IASetVertexBuffers( 0, 1, &it->vb[it->cur_target_buffer], &stride, &offset );
		device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
		device->DrawAuto();
	}

}
//=======================================================
void PESquare::Free()
{
	S_DELETE( advance_particles );
	S_DELETE( render_particles );
	S_RELEASE( vb_launcher );
}
//=======================================================
//void PESquare::RegisterEmitter( ParticleEmitter* emitter )
//{
//}
////=======================================================
//void PESquare::RemoveEmitter( ParticleEmitter* emitter )
//{
//}


}