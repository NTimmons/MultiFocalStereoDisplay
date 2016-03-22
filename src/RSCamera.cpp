#include "RenderScene.h"
#include <iostream>


void Camera::Init ( 	glm::vec3 _eye, glm::vec3 _pos, glm::vec3 _up,
	 						float _fov	, float _aspect	, float _near	, float _far)
{
	InitView(_eye, _pos, _up);
	InitProj(_fov, _aspect, _near, _far);
}

void Camera::InitView( 	glm::vec3 _eye, glm::vec3 _pos, glm::vec3 _up	)
{
	m_view = glm::lookAt(_eye, _pos, _up);
}

void Camera::InitProj( 	float _fov	, float _aspect	, float _near	, float _far)
{
	m_proj =  glm::perspective(_fov, _aspect, _near, _far);
}
	
glm::mat4 Camera::GetMVP( 	glm::mat4& _model)
{
	return m_proj * m_view * _model;
}
