#ifndef _INCL_CAMERA
#define _INCL_CAMERA

#include "VectorTypes.h"

class Camera
{
	public:
	Camera(){}

	void Init 			( 	glm::vec3 _eye, glm::vec3 _pos, glm::vec3 _up,
	 						float _fov	, float _aspect	, float _near	, float _far);

	void InitView		( 	glm::vec3 _eye, glm::vec3 _pos, glm::vec3 _up	);
	void InitProj		( 	float _fov	, float _aspect	, float _near	, float _far);
	void InitObliqueProj( float _left	, float _right	, float _bottom	, float _top, float _near, float _far );
	void InitOffAxisProj( float _left	, float _right	, float _bottom	, float _top, float _near, float _far );
	void SetProjectionMatrix( glm::mat4& _mat);
	void SetViewMatrix	( glm::mat4& _mat);

	void Rotate			( 	glm::mat4 _rotmat	);
	void Translate		( 	float _x, float _y, float _z	);
	void ChangeFOV		( 	float _amount );
	
	glm::mat4 GetMVP	( 	glm::mat4& _model);
	glm::mat4 GetMV		( 	glm::mat4& _model);
	glm::mat4 GetV		() { return m_view;}
	glm::mat4 GetP		() { return m_proj;}

	glm::vec3 GetPos    () { return m_pos;}
	glm::vec3 GetDir    () { return m_dir;}

	float m_fov;
	float m_aspect;
	float m_near;
	float m_far;

	glm::mat4 m_view;
	glm::mat4 m_proj;

	glm::vec3 m_centre;
	glm::vec3 m_up;
	glm::vec3 m_pos;
	glm::vec3 m_dir;
};


#endif