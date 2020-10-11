#include "PELaser.h"
#include "particleEmitter.h"
#include "shader.h"
#include "Graphics.h"
#include "DebugDiagnostic.h"

#define MAX_PARTICLES_PER_LASER		300
float boss_ray_color[4] = {0.05f, 0.05f, 0.05f, -1.0f };

namespace Framework
{

//=======================================================
PELaser::PELaser( ID3D10Device* _device ) : ParticleSystem(_device)
{
	line_list = NULL;
	line_list2 = NULL;
}
//=======================================================
PELaser::~PELaser()
{
	Free();
}
//=======================================================
void PELaser::Initialize()
{
	advance_particles = new Shader;
	advance_particles->Initialize( device, "shaders\\advanceParticlesLaser.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1|VERTEX_ELEMENT_POSITION2|VERTEX_ELEMENT_PER_VERTEX_FLAG );
	global_time_ptr = advance_particles->GetVariableByName("global_time")->AsScalar();
	elapsed_time_ptr = advance_particles->GetVariableByName("elapsed_time")->AsScalar();
	start_point_ptr = advance_particles->GetVariableByName("start_point")->AsVector();
	end_point_ptr = advance_particles->GetVariableByName("end_point")->AsVector();
	state_ptr = advance_particles->GetVariableByName("state")->AsScalar();
	colliding_side_ptr = advance_particles->GetVariableByName("colliding_side")->AsScalar();
	random_texture_ptr = advance_particles->GetVariableByName("random_texture")->AsShaderResource();

	render_particles = new Shader;
	render_particles->Initialize( device, "shaders\\particleShaderLaser.fx", VERTEX_ELEMENT_POSITION0|VERTEX_ELEMENT_POSITION1|VERTEX_ELEMENT_POSITION2|VERTEX_ELEMENT_PER_VERTEX_FLAG );
	color_ptr = render_particles->GetVariableByName("color")->AsVector();

	InitializeLauncherVB( 1 );
	HRESULT hr;
	D3D10_BUFFER_DESC vb_desc;

	ZeroMemory( &vb_desc, sizeof( vb_desc ) );
	vb_desc.Usage = D3D10_USAGE_DEFAULT;
	vb_desc.ByteWidth = sizeof( ParticleVertex ) * MAX_PARTICLES_PER_LASER;
	vb_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
	vb_desc.CPUAccessFlags = 0;
	vb_desc.MiscFlags = 0;
	
	lasers[0] = new Laser[MAX_LASERS];
	lasers[1] = new Laser[MAX_LASERS];
	for ( int i = 0; i < MAX_LASERS; ++i )
	{
		hr = device->CreateBuffer( &vb_desc, NULL, &(lasers[0][i].vb_laser[0]) );
		ErrorIf( FAILED(hr), "failed building vertex buffer for particles" );

		hr = device->CreateBuffer( &vb_desc, NULL, &(lasers[0][i].vb_laser[1]) );
		ErrorIf( FAILED(hr), "failed building vertex buffer for particles" );
		
		hr = device->CreateBuffer( &vb_desc, NULL, &(lasers[1][i].vb_laser[0]) );
		ErrorIf( FAILED(hr), "failed building vertex buffer for particles" );

		hr = device->CreateBuffer( &vb_desc, NULL, &(lasers[1][i].vb_laser[1]) );
		ErrorIf( FAILED(hr), "failed building vertex buffer for particles" );
	}
	cur_target_buffer = 0;
	random_texture_ptr->SetResource( GRAPHICS->GetRandomTexture() );
	initializing = true;

}
//=======================================================
void PELaser::Update( float timeslice )
{
	ParticleVertex* mapped_data;

	unsigned int stride = sizeof( ParticleVertex );
	unsigned int offset = 0;
	ID3D10Buffer* target;
	
	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	global_time_ptr->SetInt((unsigned int)timeGetTime());
	elapsed_time_ptr->SetFloat( timeslice );
	if ( initializing )
	{
		initializing = false;
		for ( int i = 0; i < MAX_LASERS; ++i )
		{
			vb_launcher->Map( D3D10_MAP_WRITE_DISCARD, 0, (void**)&mapped_data );
			mapped_data[0].v0.x = (float)(i * 5);
			mapped_data[0].v0.y = 0.0f;
			mapped_data[0].type = 0;
			vb_launcher->Unmap();
			advance_particles->Apply(device);
			device->IASetVertexBuffers( 0, 1, &vb_launcher, &stride, &offset );
			target = lasers[0][i].vb_laser[cur_target_buffer];
			device->SOSetTargets( 1, &target, &offset );
			device->Draw( 1, 0 );

			target = lasers[1][i].vb_laser[cur_target_buffer];
			device->SOSetTargets( 1, &target, &offset );
			device->Draw( 1, 0 );
		}
	}
	else
	{
		LineListType::iterator it;
		// draw line list 1
			//printf ( "%i\n", line_list->size() );
		if ( line_list != NULL )
		{
			it = line_list->begin();
			for ( int i = 0; i < MAX_LASERS; ++i )
			{
				int state = 0;
				int colliding_side = 0;
				if ( it != line_list->end() )
				{
					//if ( i == 4 ) printf("%0.2f %0.2f %0.2f %0.2f\n", it->start.x, it->start.y, it->end.x, it->end.y );
					if ( i == 0 ) state = 1;
					else state = 2;
					start_point_ptr->SetFloatVector( it->start );
					end_point_ptr->SetFloatVector( it->end );
					if ( !(it->visible) ) state = 0;	// invisible rays
					colliding_side = it->colliding_side;
					++it;
				}
				if ( state == 0 && i > 0 ) state = -1;
				state_ptr->SetInt( state );
				colliding_side_ptr->SetInt( colliding_side );
				advance_particles->Apply( device );
				device->IASetVertexBuffers( 0, 1, &lasers[0][i].vb_laser[cur_target_buffer], &stride, &offset );
				target = lasers[0][i].vb_laser[1 - cur_target_buffer];
				device->SOSetTargets( 1, &target, &offset );

				device->DrawAuto();
			}
		}
		// draw line list 2
		if ( line_list2 != NULL )
		{
			it = line_list2->begin();
			for ( int i = 0; i < MAX_LASERS; ++i )
			{
				int state = -2;
				int colliding_side = 0;
				if ( it != line_list2->end() )
				{
					if ( i == 0 ) state = 1;
					else state = 2;
					start_point_ptr->SetFloatVector( it->start );
					end_point_ptr->SetFloatVector( it->end );
					if ( !(it->visible) ) state = 0;	// invisible rays
					colliding_side = it->colliding_side;
					++it;
				}
				//if ( state == 0 && i > 0 ) state = -1;
				state_ptr->SetInt( state );
				colliding_side_ptr->SetInt( colliding_side );
				advance_particles->Apply( device );
				device->IASetVertexBuffers( 0, 1, &lasers[1][i].vb_laser[cur_target_buffer], &stride, &offset );
				target = lasers[1][i].vb_laser[1 - cur_target_buffer];
				device->SOSetTargets( 1, &target, &offset );

				device->DrawAuto();
			}
		}
		// flip vertex buffer
		cur_target_buffer = 1 - cur_target_buffer;
	}

	target = NULL;
	device->SOSetTargets( 1, &target, &offset );
}
//=======================================================
void PELaser::Draw( Mat4& mat_view, Mat4& mat_proj )
{
	unsigned int stride = sizeof( ParticleVertex );
	unsigned int offset = 0;

	device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );
	render_particles->SetTransformationConstants( device, mat_view, mat_proj );
	color_ptr->SetFloatVector( *color );
	
	render_particles->Apply( device );
	for ( int i = 0; i < MAX_LASERS; ++i )
	{
		device->IASetVertexBuffers( 0, 1, &(lasers[0][i].vb_laser[cur_target_buffer]), &stride, &offset );
		device->DrawAuto();
	}	
	
	color_ptr->SetFloatVector( boss_ray_color );
	render_particles->Apply( device );
	for ( int i = 0; i < MAX_LASERS; ++i )
	{
		device->IASetVertexBuffers( 0, 1, &(lasers[1][i].vb_laser[cur_target_buffer]), &stride, &offset );
		device->DrawAuto();
	}	
}
//=======================================================
void PELaser::Free()
{
	S_DELETE( advance_particles );
	S_DELETE( render_particles );
	S_RELEASE( vb_launcher );
	for ( int i = 0; i < MAX_LASERS; ++ i )
	{
		S_RELEASE( lasers[0][i].vb_laser[0] );
		S_RELEASE( lasers[0][i].vb_laser[1] );
		S_RELEASE( lasers[1][i].vb_laser[0] );
		S_RELEASE( lasers[1][i].vb_laser[1] );
	}
	delete [] lasers[0];
	delete [] lasers[1];
}

}
