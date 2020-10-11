#include "Precompiled.h"
#include "camera.h"


namespace Framework
{


//================================================================================
Camera::Camera()
{
//	camera_vector[0] = camera_vector[1] = camera_vector[2] = 0.0f;
//	focus_point[0] = focus_point[1] = focus_point[2] = 0.0f;
	D3DXMatrixIdentity( &matrix_proj );
//	D3DXMatrixIdentity( &matrix_rotation );
//	D3DXMatrixIdentity( &matrix_rotation_inverse );

	camera_position[0] = camera_position[1] = camera_position[2] = 0.0f;
	lookat_position[0] = lookat_position[1] = lookat_position[2] = 0.0f;
}
//================================================================================
Camera::~Camera()
{
}
//================================================================================
void Camera::Serialize( ISerializer& stream )
{
	stream.readValue( "camera_position", static_cast<float*>(camera_position), 3 );
	stream.readValue( "lookat_position", static_cast<float*>(lookat_position), 3 );
	//stream.readValue( "camera_vector", camera_vector, 3 );
	//stream.readValue( "focus_point", focus_point, 3 );
	//D3DXVECTOR3 default_rotation;
	//if ( stream.readValue( "default_rotation", reinterpret_cast<float*>(&default_rotation), 3 ) )
	//{
	//	RotateCameraSpace( default_rotation[0], default_rotation[1], default_rotation[2] );
	//}
}
//================================================================================
void Camera::Initialize()
{
	need_for_rebuild = true;
}
//================================================================================
void Camera::GetViewProjMatrix( void* storage )
{
	if ( need_for_rebuild ) RebuildMatrices();
	memcpy( storage, static_cast<void*>( &matrix_viewproj ), sizeof(D3DXMATRIX) );
}
//================================================================================
void Camera::GetViewMatrix( void* storage )
{
	if ( need_for_rebuild ) RebuildMatrices();
	memcpy( storage, static_cast<void*>( &matrix_view ), sizeof(D3DXMATRIX) );
}
//================================================================================
void Camera::GetProjMatrix( void* storage )
{
	D3DXMATRIX* result = static_cast<D3DXMATRIX*>(storage);
	memcpy( storage, static_cast<void*>( &matrix_proj ), sizeof(D3DXMATRIX) );
}
//================================================================================
void Camera::RebuildMatrices()
{
	D3DXVECTOR3 viewing_vector = lookat_position - camera_position;//( lookat_position[0]-camera_position[0], lookat_position[1]-camera_position[1], lookat_position[2]-camera_position[2] );
	D3DXVECTOR3 up( 0.0f, 1.0f, 0.0f );
	D3DXVec3Cross( &left, &viewing_vector, &up );
	D3DXVec3Normalize( &left, &left );

	//D3DXMatrixTranslation( &matrix_view, -focus_point[0], -focus_point[1], -focus_point[2] );
	//D3DXMatrixMultiply( &matrix_view, &matrix_view, &matrix_rotation );
	//matrix_view._41 -= camera_vector[0];
	//matrix_view._42 -= camera_vector[1];
	//matrix_view._43 -= camera_vector[2];
	//D3DXMatrixTranspose( &matrix_view, &matrix_view );

	D3DXMatrixLookAtLH( &matrix_view, &camera_position, &lookat_position, &up );
	D3DXMatrixTranspose( &matrix_view, &matrix_view );

	D3DXMatrixMultiply( &matrix_viewproj, &matrix_proj, &matrix_view );

	need_for_rebuild = false;

}
//================================================================================
void Camera::UpdateProjectionOrtho( float width, float height, float znear, float zfar )
{
	D3DXMatrixOrthoLH( &matrix_proj, width, height, znear, zfar );
	D3DXMatrixTranspose( &matrix_proj, &matrix_proj );
	need_for_rebuild = true;
}
//================================================================================
void Camera::UpdateProjectionFrustum( float fov, float ratio, float znear, float zfar )
{
	D3DXMatrixPerspectiveFovLH( &matrix_proj, fov, ratio, znear, zfar );
	D3DXMatrixTranspose( &matrix_proj, &matrix_proj );
	need_for_rebuild = true;
}
//================================================================================
void Camera::RotateCameraSpace( float x, float y, float z )
{
	//D3DXMATRIX localrotation;
	//D3DXMatrixRotationYawPitchRoll( &localrotation, y, x, z );
	//D3DXMatrixMultiply( &matrix_rotation, &matrix_rotation, &localrotation );
	//D3DXMatrixTranspose( &matrix_rotation_inverse, &matrix_rotation );

	camera_position[1] += y;
	float c = cos(x), s = sin(x);
	float ox = camera_position.x - lookat_position.x, oy = camera_position.z - lookat_position.z;
	camera_position.x = c * ox - s * oy + lookat_position.x;
	camera_position.z = s * ox + c * oy + lookat_position.z;

	need_for_rebuild = true;
}
//================================================================================
void Camera::TranslateCameraSpace( float x, float y, float z )
{
//	D3DXVECTOR4 vec( x, y, z, 1.0f );
//	D3DXVec4Transform( &vec, &vec, &matrix_rotation_inverse );

	camera_position += x * left;
	lookat_position += x * left;

	D3DXVECTOR3 zvector( lookat_position.x - camera_position.x, 0.0f, lookat_position.z - camera_position.z );
	camera_position += z * zvector;
	lookat_position += z * zvector;

	camera_position.y += y;
	lookat_position.y += y;

	need_for_rebuild = true;
}
//================================================================================
void Camera::GetEyePosition( float& x, float& y, float& z )
{
	/*		previous code
	D3DXVECTOR4 vec( camera_vector[0], camera_vector[1], camera_vector[2], 1.0f );
	D3DXVec4Transform( &vec, &vec, &matrix_rotation_inverse );
	x = vec[0] + focus_point[0];
	y = vec[1] + focus_point[1];
	z = vec[2] + focus_point[2];
	*/
	x = camera_position[0];
	y = camera_position[1];
	z = camera_position[2];
}


}