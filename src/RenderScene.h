#ifndef _INCL_RENDER_SCENE
#define _INCL_REDER_SCENE

#include <vector>

#include <GL/glew.h>
#include "GL/freeglut.h"

#include <string>
#include <fstream>


struct colour
{
	colour(float& _r, float& _g, float& _b, float& _a) : r(_r), g(_g), b(_b), a(_a){}
	colour(): r(0.f), g(0.f), b(0.f), a(0.f){}

	float R(){return r;} const
	float G(){return g;} const
	float B(){return b;} const
	float A(){return a;} const

	float r;
	float g;
	float b; 
	float a;
};

struct position
{
	position(float& _x, float& _y, float& _z ) : x(_x), y(_y), z(_z){}
	position( ): x(0.f), y(0.f), z(0.f){}

	float X(){return x;}const
	float Y(){return y;}const
	float Z(){return z;}const

	float x;
	float y;
	float z;
};

struct size
{
	size(const float& _w,const float& _h): w(_w), h(_h){}
	size(): w(0.f), h(0.f){}

	float W(){return w;}const
	float H(){return h;}const

	float w;
	float h;
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
	Screen(position _pos, size _size, colour _colour):m_pos(_pos), m_size(_size), m_colour(_colour){}

	position m_pos;
	size 	 m_size;
	colour   m_colour;
};

struct ShaderProgram
{
	ShaderProgram(GLuint _id): programID(_id){}

	//std::string someString;

	GLuint programID;
};

class RenderScene
{
public:
	RenderScene(){}

	//Setup
	void Initialise();

	//Rendering
	void Render();

	//Screen Control
	void InitialiseScreenPositions();


	// FBO Control
	FBO  CreateFrameBuffer(int _width, int _height, GLenum _format);
	void ApplySingleFrameBuffer(FBO _fbo);
	void ApplyFourFrameBuffers(FBO _fbo0, FBO _fbo1,FBO _fbo2,FBO _fbo3);
	void ClearFrameBuffers();

	// Shader Management
	private:
	int 			GetFileLength				(std::ifstream& file);
	int 			LoadShader					(char* filename, char** ShaderSource, int* len);
	int 			UnloadShader				(GLubyte** ShaderSource);

	bool 			validateCompilation			(GLuint _shader);
	bool 			validateLinking				(GLuint program, GLuint vertexShader, GLuint fragmentShader);

	public:
	ShaderProgram* 	CreateShaderProgramObject	(char* _vertexFilename, char* _pixelFilename);

	// Lists and lists and lists
	std::vector<GLuint> 		m_frameBuffers;
	std::vector<GLuint> 		m_renderTextures;
	std::vector<GLuint> 		m_depthRenderTextures;
	std::vector<FBO> 			m_FBOList;
	std::vector<ShaderProgram> 	m_shaderList;

	Screen						m_screenArray[4];

	//Window Controls
	int m_PosX;
	int m_PosY;

	int m_SizeX;
	int m_SizeY;

};


#endif






