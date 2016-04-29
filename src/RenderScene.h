#ifndef _INCL_RENDER_SCENE
#define _INCL_REDER_SCENE

#include <vector>

#include <GL/glew.h>
#include "GL/freeglut.h"

#include <fstream>
#include <iostream>
#include <map>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define TESTGL 	RenderScene::TestGLError(__FILE__, __LINE__)

//TODO NEEDS REFACTORING

struct colour
{
	colour(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a){}
	colour(): r(0.f), g(0.f), b(0.f), a(0.f){}

	float R(){return r;} 
	float G(){return g;} 
	float B(){return b;} 
	float A(){return a;} 

	float r;
	float g;
	float b; 
	float a;
};

struct Position
{
	Position(float _x, float _y, float _z )
	{
		pos[0] = _x;
		pos[1] = _y;
		pos[2] = _z;
		pos[3] = 0.0f;
	}
	Position( ) 
	{
		pos[0] = 0.f;
		pos[1] = 0.f;
		pos[2] = 0.f;
		pos[3] = 0.f;
	}

	float* GetPtr(){return &pos[0];}
	float X(){return pos[0];}
	float Y(){return pos[1];}
	float Z(){return pos[2];}

	float pos[4];

};

struct size
{
	size(const float _w,const float _h): w(_w), h(_h){}
	size(): w(0.f), h(0.f){}

	float W(){return w;}
	float H(){return h;}

	float w;
	float h;
};

struct texCoord
{
	texCoord():u(0.f), v(0.f){}
	texCoord(float _u, float _v): u(_u), v(_v){}

	float u;
	float v;

};



class Texture
{
public:
	Texture(): m_texture(-1){}

	void Init(std::string& _path);
	
	GLuint Get(){ return m_texture; }

	GLuint m_texture;
};


struct FBOMulti
{
	FBOMulti()
	:	m_frameBufferIndex(-1), 
		m_renderTextureIndex0(-1), m_renderTextureIndex1(-1),
		 m_depthRenderIndex(-1), m_width(0), m_height(0){}
	FBOMulti(GLuint _fbi, GLuint _rti0, GLuint _rti1,GLuint _dri, int _width, int _height)
	: 	m_frameBufferIndex(_fbi),
		m_renderTextureIndex0(_rti0), m_renderTextureIndex1(_rti1),
		m_depthRenderIndex(_dri), m_width(_width), m_height(_height){}

	GLuint  m_frameBufferIndex;
	GLuint  m_renderTextureIndex0;
	GLuint  m_renderTextureIndex1;
	GLuint  m_depthRenderIndex;
	int 	m_width;
	int 	m_height;
};

struct FBO
{
	FBO():m_frameBufferIndex(-1), m_renderTextureIndex(-1), m_depthRenderIndex(-1), m_width(0), m_height(0){}
	FBO(GLuint _fbi, GLuint _rti, GLuint _dri, int _width, int _height): m_frameBufferIndex(_fbi), m_renderTextureIndex(_rti), m_depthRenderIndex(_dri), m_width(_width), m_height(_height){}

	GLuint  m_frameBufferIndex;
	GLuint  m_renderTextureIndex;
	GLuint  m_depthRenderIndex;
	int 	m_width;
	int 	m_height;
};

struct Screen
{
	Screen(){}
	Screen(Position _pos, size _size, colour _colour):m_pos(_pos), m_size(_size), m_colour(_colour){}

	Position m_pos;
	size 	 m_size;
	colour   m_colour;
};

class ScreenLayout
{
	public:
	ScreenLayout();
	
	Position 	GetScreenPos					( unsigned int _index);
	size     	GetScreenSize					( unsigned int _index);

	void 		AdjustScreenPos					( Position 	_offset, unsigned int _index);
	void 		AdjustScreenSize				( size 		_offset, unsigned int _index);

	void 		SetScreenPos		( unsigned int _index, Position& _pos);
	void     	SetScreenSize		( unsigned int _index, size& _size);

	private:
	Screen		m_screenArray[4];

};


class ShaderProgram
{
	public:
	ShaderProgram(GLuint _id): programID(_id), name("Unnamed"){}
	ShaderProgram(GLuint _id, std::string& _name): programID(_id), name(_name){}

	GLint FetchUniformValue(std::string _name);

	void SetUniform1UI(std::string _name, GLuint _val);
	void SetUniform1F(std::string _name, float _val);
	void SetUniform2F(std::string _name, float _val0, float _val1);
	void SetUniform3F(std::string _name, float _val0, float _val1, float _val2);
	void SetUniform4F(std::string _name, float _val0, float _val1, float _val2, float _val3);

	void SetUniform2FP(std::string _name, float* _val);
	void SetUniform3FP(std::string _name, float* _val);
	void SetUniform4FP(std::string _name, float* _val);

	void SetMatrix4FV(std::string _name, const GLfloat* _val);

	std::map<std::string, GLint> m_uniformMap;	
	GLuint programID;
	std::string name;
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
};

class Mesh
{
public:

	virtual void Draw() = 0;

	GLuint vertexBufferObject;
	GLuint elementBufferObject;
};


struct VBOIBO
{
	VBOIBO(GLuint _vbo, GLuint _ibo, unsigned int _count)
	: vertexBufferObject(_vbo), elementBufferObject(_ibo), elementCount(_count){}

	GLuint 		 vertexBufferObject;
	GLuint 		 elementBufferObject;
	unsigned int elementCount;
};

class AIMesh : public Mesh
{
public:

	AIMesh()
	{
	}

	void Initialise	(std::string& _path);
	void Draw		();

	std::vector<VBOIBO> m_vertexObjects;
};

#ifndef offsetof
#define offsetof(type,member) ((std::size_t) &(((type*)0)->member))
#endif

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


class Camera
{
	public:
	Camera(){}

	void Init 			( 	glm::vec3 _eye, glm::vec3 _pos, glm::vec3 _up,
	 						float _fov	, float _aspect	, float _near	, float _far);

	void InitView		( 	glm::vec3 _eye, glm::vec3 _pos, glm::vec3 _up	);
	void InitProj		( 	float _fov	, float _aspect	, float _near	, float _far);
	
	glm::mat4 GetMVP	( 	glm::mat4& _model);

	glm::mat4 m_view;
	glm::mat4 m_proj;
};


class RenderScene
{
public:
	RenderScene(){ m_activeScreen = 0;}

	//Setup
	void Initialise();
	void InitialiseRenderObjects();

	static void TestGLError(const char* _file, int _line);

	//Rendering
	void Render();
	void Render_CopyToViews();
	void Render_Scene();

	void SceneBody();

	void RenderQuad(Position& _pos, size& _size, colour& _col);
	void RenderScreenQuadAtOffset(Position& _offset, size& _size);

	//Screen Control
	void InitialiseScreenPositions();
	void HandleInput( unsigned char _key);

	bool ScreenLoadFromFile	(std::string _path, ScreenLayout* _screen);
	void ScreenWriteToFile	(std::string _path, ScreenLayout* _screen);


	// FBO Control
	FBO 	 CreateSingleFrameBuffer(int _width, int _height, GLenum _format);
	FBOMulti CreateTwoFrameBuffer(int _width, int _height, GLenum _format);
	
	void ApplySingleFrameBuffer	(FBO& _fbo);
	void ApplyTwoFrameBuffers(FBOMulti _fbo);
	void ClearFrameBuffers		( );

	// Shader Management
	private:
	int 	GetFileLength		(std::ifstream& file);
	int 	LoadShader			(std::string& filename, std::string& ShaderSource, int* len);
	int 	UnloadShader		(GLubyte** ShaderSource);

	bool	validateCompilation	(GLuint _shader);
	bool	validateLinking		(GLuint program, GLuint vertexShader, GLuint fragmentShader);

	public:
	bool 	CreateShaderProgramObject	(std::string& _vertexFilename, std::string& _pixelFilename, std::string _name);

	// Lists and lists and lists
	std::vector<GLuint> 		m_frameBuffers;
	std::vector<GLuint> 		m_renderTextures;
	std::vector<GLuint> 		m_depthRenderTextures;
	std::vector<FBO> 			m_FBOList;
	std::vector<FBOMulti> 		m_FBOMultiList;
	
	std::map<std::string, ShaderProgram> m_shaderMap;

	ScreenLayout				m_layoutControl;

	QuadMesh					m_genericUnitQuad;
	AIMesh						m_boxMesh;
	Texture						m_testTexture;

	//Render Settings
	Camera 						m_camera[2];

	//Window Controls
	int m_PosX;
	int m_PosY;

	int m_SizeX;
	int m_SizeY;
	
	int m_activeScreen;

};


#endif






