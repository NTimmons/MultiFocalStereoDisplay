#include "RenderScene.h"
#include <iostream>


void Camera::Init ( 	glm::vec3 _eye, glm::vec3 _pos, glm::vec3 _up,
	 						float _fov	, float _aspect	, float _near	, float _far)
{
	InitView(_eye, _pos, _up);
	InitProj(_fov, _aspect, _near, _far);

	m_fov = _fov;
	m_aspect = _aspect;
	m_near = _near;
	m_far = _far;

	m_pos = _eye;
	m_centre = _pos;
	m_up = _up;
	m_dir = glm::normalize(_pos - _eye);
}

void Camera::InitView( 	glm::vec3 _eye, glm::vec3 _pos, glm::vec3 _up	)
{
	m_view = glm::lookAt(_eye, _pos, _up);
}


void Camera::InitProj( 	float _fov	, float _aspect	, float _near	, float _far)
{
	m_fov = _fov;
	m_aspect = _aspect;
	m_near = _near;
	m_far = _far;

	m_proj =  glm::perspectiveFov(_fov, 2048.f, 1536.f, _near, _far);
}

void Camera::InitObliqueProj ( float _left	, float _right	, float _bottom	, float _top, float _near, float _far )          
{
	//m_proj = glm::frustum(-1.0f, -1.0f, -1.0f, 1.0f, 1.f, 150.f);
	m_proj = glm::ortho(_left, _right, _bottom, _top, _near, _far);
}

void Camera::InitOffAxisProj ( float _left	, float _right	, float _bottom	, float _top, float _near, float _far )          
{
	//m_proj = glm::frustum(-1.0f, -1.0f, -1.0f, 1.0f, 1.f, 150.f);
	m_proj = glm::frustum(_left, _right, _bottom, _top, _near, _far);
}

void Camera::SetProjectionMatrix( glm::mat4& _mat)
{
	m_proj = _mat;
}

void Camera::SetViewMatrix( glm::mat4& _mat)
{
	m_view = _mat;
}



void Camera::ChangeFOV( float _amount )
{

	m_fov += _amount;
	
	m_fov = m_fov < 1.f ? 1.f : m_fov;

	std::cerr << "New FOV = " << m_fov << " \n";

	InitProj(m_fov, m_aspect, m_near, m_far);

}

void Camera::Rotate( 	glm::mat4 _rotmat	)
{
	glm::mat4 translate = glm::translate(glm::mat4(1.f), -m_centre);
	glm::mat4 translateback = glm::translate(glm::mat4(1.f), m_centre);
	glm::vec4 newEyePos = translateback * (_rotmat * ( translate * glm::vec4(m_pos.x, m_pos.y, m_pos.z, 1.0f) ) );
	m_pos = glm::vec3(newEyePos.x, newEyePos.y, newEyePos.z);
	m_dir = glm::normalize(m_centre - m_pos);

	m_view = glm::lookAt(m_pos, m_centre, m_up);
}

void Camera::Translate( 	float _x, float _y, float _z	)
{
	m_pos.x += _x;
	m_pos.y += _y;
	m_pos.z += _z;

	m_centre.x += _x;
	m_centre.y += _y;
	m_centre.z += _z;

	m_dir = glm::normalize(m_centre - m_pos);

	m_view = glm::lookAt(m_pos, m_centre, m_up);
}


glm::mat4 Camera::GetMV( 	glm::mat4& _model)
{
	return m_view * _model;
}
	
glm::mat4 Camera::GetMVP( 	glm::mat4& _model)
{
	return m_proj * m_view * _model;
}
