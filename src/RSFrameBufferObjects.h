#ifndef _INCL_FBO
#define _INCL_FBO

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

#endif