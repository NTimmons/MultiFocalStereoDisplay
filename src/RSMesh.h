#ifndef _INCL_MESH
#define _INCL_MESH

#ifndef offsetof
#define offsetof(type,member) ((std::size_t) &(((type*)0)->member))
#endif

#include "VectorTypes.h"

struct VBOIBO
{
	VBOIBO(GLuint _vbo, GLuint _ibo, unsigned int _count)
	: vertexBufferObject(_vbo), elementBufferObject(_ibo), elementCount(_count){}

	GLuint 		 vertexBufferObject;
	GLuint 		 elementBufferObject;
	unsigned int elementCount;
};

// 4 + 4 + 2 + 2 = 48byte vertex. 
struct QuadVertex
{
	Position m_pos;
	colour	 m_col;
	size	 m_size;
	texCoord m_uv;
};

struct GenericVertex
{
	Position m_pos;
	Position m_nor;
	texCoord m_uv;
	texCoord m_uv1;
	Position m_tan;
};

class Mesh
{
public:

	virtual void Draw() = 0;

	GLuint vertexBufferObject;
	GLuint elementBufferObject;
};

class AIMesh : public Mesh
{
public:

	AIMesh()
	{
		m_rotationMat= glm::mat4(1.f);
		m_scaleMat= glm::mat4(1.f);
		m_transMat= glm::mat4(1.f);
	}

	void Initialise	(std::string& _path);
	void Draw		();

	void SetRotationMat (glm::mat4 _mat)
	{
		m_rotationMat = _mat;
	}

	glm::mat4 GetRotationMat ()
	{
		return m_rotationMat;
	}

	void SetTranslationMat (glm::mat4 _mat)
	{
		m_transMat = _mat;
	}

	void SetScaleMat (glm::mat4 _mat)
	{
		m_scaleMat = _mat;
	}

	glm::mat4 GetModelMat ()
	{
		return m_transMat * m_rotationMat * m_scaleMat;
	}

	void SetName(std::string _name)
	{
		m_name = _name;
	}

	void SetMaterial(std::string _name)
	{
		m_material = _name;
	}

	std::string GetMaterial()
	{
		return m_material;
	}

	std::string GetName()
	{
		return m_name;
	}	

	std::string m_name;
	std::string m_material;

	glm::mat4 m_rotationMat;
	glm::mat4 m_scaleMat;
	glm::mat4 m_transMat;

	std::vector<VBOIBO> m_vertexObjects;
};



class QuadMesh : public Mesh
{
public:

	QuadMesh()
	{
	}

	void Initialise();
	void Draw();

	QuadVertex 		vertices[4];
	unsigned int	indicies[6];
};



#endif