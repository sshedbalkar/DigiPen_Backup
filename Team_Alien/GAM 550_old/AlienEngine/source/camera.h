#pragma once //Makes sure this header is only included once

#include "Composition.h"

namespace Framework
{
	///The transform component provides a shared position and rotation.
	class Camera : public GameComponent
	{
		//D3DXMATRIX matrix_rotation;
		//D3DXMATRIX matrix_rotation_inverse;
		//float camera_vector[3];
		//float focus_point[3];

		/////// ignore above codes
		D3DXMATRIX matrix_proj;
		D3DXMATRIX matrix_view;
		D3DXMATRIX matrix_viewproj;
		D3DXVECTOR3 camera_position;
		D3DXVECTOR3 lookat_position;
		D3DXVECTOR3 left;

		bool need_for_rebuild;

	public:	
		Camera();
		virtual ~Camera();
		virtual void Serialize(ISerializer& str);
		virtual void Initialize();

		void GetViewProjMatrix( void* storage );
		void GetViewMatrix( void* storage );
		void GetProjMatrix( void* storage );
		void UpdateProjectionOrtho( float width, float height, float znear, float zfar );
		void UpdateProjectionFrustum( float fov, float ratio, float znear, float zfar );
		void RotateCameraSpace( float x, float y, float z );
		void TranslateCameraSpace( float x, float y, float z );
		void GetEyePosition( float& x, float& y, float& z );

	private:
		void RebuildMatrices();
	};
}
