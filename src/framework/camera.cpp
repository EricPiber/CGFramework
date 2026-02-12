#include "camera.h"

#include "main/includes.h"
#include <iostream>

Camera::Camera()
{
	view_matrix.SetIdentity();
	SetOrthographic(-1,1,1,-1,-1,1);
}

Vector3 Camera::GetLocalVector(const Vector3& v)
{
	Matrix44 iV = view_matrix;
	if (iV.Inverse() == false)
		std::cout << "Matrix Inverse error" << std::endl;
	Vector3 result = iV.RotateVector(v);
	return result;
}

Vector3 Camera::ProjectVector(Vector3 pos)
{
	Vector4 pos4 = Vector4(pos.x, pos.y, pos.z, 1.0);
	Vector4 result = viewprojection_matrix * pos4;
	if (type == ORTHOGRAPHIC)
		return result.GetVector3();
	else
		return result.GetVector3() / result.w;
}

void Camera::Rotate(float angle, const Vector3& axis)
{
	Matrix44 R;
	R.MakeRotationMatrix(angle, axis);
	Vector3 new_front = R * (center - eye);
	center = eye + new_front;
	UpdateViewMatrix();
}

void Camera::Move(Vector3 delta)
{
	Vector3 localDelta = GetLocalVector(delta);
	eye = eye - localDelta;
	center = center - localDelta;
	UpdateViewMatrix();
}

void Camera::SetOrthographic(float left, float right, float top, float bottom, float near_plane, float far_plane)
{
	type = ORTHOGRAPHIC;

	this->left = left;
	this->right = right;
	this->top = top;
	this->bottom = bottom;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	UpdateProjectionMatrix();
}

void Camera::SetPerspective(float fov, float aspect, float near_plane, float far_plane)
{
	type = PERSPECTIVE;

	this->fov = fov;
	this->aspect = aspect;
	this->near_plane = near_plane;
	this->far_plane = far_plane;

	UpdateProjectionMatrix();
}

void Camera::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
{
	this->eye = eye;
	this->center = center;
	this->up = up;

	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	// Reset Matrix (Identity)
	view_matrix.SetIdentity();

	// Comment this line to create your own projection matrix!
	//SetExampleViewMatrix();

	// Remember how to fill a Matrix4x4 (check framework slides)
	// Careful with the order of matrix multiplications, and be sure to use normalized vectors!
	
	// Create the view matrix rotation
    Matrix44 Rt = GetFRUMatix();
    Rt.Transpose();
    
	// ...
	// view_matrix.M[3][3] = 1.0;

	// Translate view matrix
    Matrix44 T = Matrix44();
    T.Set(
          1, 0, 0, -eye.x,
          0, 1, 0, -eye.y,
          0, 0, 1, -eye.z,
          0, 0, 0, 1
          );
    view_matrix = Rt * T;  // inverse of the camera to world matrix
    
	UpdateViewProjectionMatrix();
}

// Create a projection matrix
void Camera::UpdateProjectionMatrix()
{
	// Reset Matrix (Identity)
	projection_matrix.SetIdentity();

	// Comment this line to create your own projection matrix!
	//SetExampleProjectionMatrix();

	// Remember how to fill a Matrix4x4 (check framework slides)
	
    
	if (type == PERSPECTIVE) {  // Create the projection matrix for perspective projection
		projection_matrix.Set(
			1.0f/(aspect*tanf(DEG2RAD*fov/2.0f)), 0, 0, 0,
			0, 1.0f/tanf(DEG2RAD*fov/2.0f), 0, 0,
			0, 0, (far_plane+near_plane)/(near_plane-far_plane), (2.0f*far_plane*near_plane)/(near_plane - far_plane),
			0, 0, -1.0f, 0
		);
	}
	else if (type == ORTHOGRAPHIC) {  // Create the projection matrix for orthographic projection
		projection_matrix.Set(
			2.0f/(right-left), 0, 0, -(right+left)/(right-left),
			0, 2.0f/(top-bottom), 0, -(top+bottom)/(top-bottom),
			0, 0, -2.0f/(far_plane-near_plane), -(far_plane+near_plane)/(far_plane-near_plane),
			0, 0, 0, 1
		);
	}

	UpdateViewProjectionMatrix();
}

void Camera::UpdateViewProjectionMatrix()
{
	viewprojection_matrix = projection_matrix * view_matrix;
}

Matrix44 Camera::GetViewProjectionMatrix()
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();

	return viewprojection_matrix;
}

// The following methods have been created for testing.
// Do not modify them.

void Camera::SetExampleViewMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, view_matrix.m );
}

void Camera::SetExampleProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (type == PERSPECTIVE)
		gluPerspective(fov, aspect, near_plane, far_plane);
	else
		glOrtho(left,right,bottom,top,near_plane,far_plane);

	glGetFloatv(GL_PROJECTION_MATRIX, projection_matrix.m );
	glMatrixMode(GL_MODELVIEW);
}

Matrix44 Camera::GetFRUMatix() {  // Get the rotation matrix of the camera to world transformation
    Vector3 forward = Vector3();
    Vector3 right = Vector3();
    Vector3 local_up = Vector3();
    
    forward = eye - center;
    forward.Normalize();
    
    right = up.Cross(forward);
    right.Normalize();
    
    local_up = forward.Cross(right);
    local_up.Normalize();
    
    Matrix44 R = Matrix44();
    R.Set(
          right.x,  local_up.x, forward.x,  0,
          right.y,  local_up.y, forward.y,  0,
          right.z,  local_up.z, forward.z,  0,
          0,        0,          0,          1
          );
    return R;
}
