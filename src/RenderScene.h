#ifndef _INCL_RENDER_SCENE
#define _INCL_REDER_SCENE

#include <vector>

#include <GL/glew.h>
#include "GL/freeglut.h"

#include <fstream>
#include <iostream>
#include <map>


#define TESTGL 	TestGLError(__FILE__, __LINE__)

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
	Position(float _x, float _y, float _z ) : x(_x), y(_y), z(_z), w(0.f){}
	Position( ): x(0.f), y(0.f), z(0.f), w(0.f){}

	float X(){return x;}
	float Y(){return y;}
	float Z(){return z;}

	float x;
	float y;
	float z;
	float w;
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

struct FBO
{
	FBO():m_frameBufferIndex(-1), m_renderTextureIndex(-1), m_depthRenderIndex(-1), m_width(0), m_height(0){}
	FBO(GLuint _fbi, GLuint _rti, GLuint _dri, int _width, int _height): m_frameBufferIndex(_fbi), m_renderTextureIndex(_rti), m_depthRenderIndex(_dri), m_width(_width), m_height(_height){}

	GLuint m_frameBufferIndex;
	GLuint m_renderTextureIndex;
	GLuint m_depthRenderIndex;
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

class ShaderProgram
{
	public:
	ShaderProgram(GLuint _id): programID(_id), name("Unnamed"){}
	ShaderProgram(GLuint _id, std::string& _name): programID(_id), name(_name){}
	
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

class Mesh
{
public:

	virtual void Draw() = 0;

	GLuint vertexBufferObject;
	GLuint elementBufferObject;
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

class RenderScene
{
public:
	RenderScene(){}

	//Setup
	void Initialise();
	void InitialiseRenderObjects();


	//Rendering
	void Render();
	void RenderQuad(Position& _pos, size& _size, colour& _col);

	//Screen Control
	void InitialiseScreenPositions();


	// FBO Control
	FBO  CreateFrameBuffer		(int _width, int _height, GLenum _format);
	void ApplySingleFrameBuffer	(FBO _fbo);
	void ApplyFourFrameBuffers	(FBO _fbo0, FBO _fbo1, FBO _fbo2, FBO _fbo3);
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
	
	std::map<std::string, ShaderProgram> m_shaderMap;

	Screen						m_screenArray[4];

	QuadMesh					m_genericUnitQuad;

	//Window Controls
	int m_PosX;
	int m_PosY;

	int m_SizeX;
	int m_SizeY;


};


#endif






