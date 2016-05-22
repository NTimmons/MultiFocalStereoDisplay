#include "RenderScene.h"
#include <iostream>


FBO RenderScene::CreateSingleFrameBuffer(int _width, int _height, GLenum _format)
{
	(void)_format;

	GLuint color_tex;
	GLuint depth_rb;
	GLuint fb;

	glGenTextures(1, &color_tex);
	glBindTexture(GL_TEXTURE_2D, color_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);

	glGenRenderbuffers(1, &depth_rb);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _width, _height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		//std::cerr<<"Frame Buffer is good.\n";
	}
	else
	{
		std::cerr<<"Frame Buffer is BROKEN.\n";
	}

	FBO fbo_one(fb,color_tex, depth_rb, _width, _height) ;
	ClearFrameBuffers();

	return fbo_one;
}

FBOMulti RenderScene::CreateTwoFrameBuffer(int _width, int _height, GLenum _format)
{
	(void)_format;

	GLuint color_tex[2];
	GLuint depth_rb;
	GLuint fb;

	glGenTextures(4, &color_tex[0]);
	glBindTexture(GL_TEXTURE_2D, color_tex[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, color_tex[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex[0], 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, color_tex[1], 0);

	glGenRenderbuffers(1, &depth_rb);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _width, _height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);
	
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr<<"Frame Buffer is good.\n";
	}
	else
	{
		std::cerr<<"Frame Buffer is BROKEN.\n";
	}

	FBOMulti fbo_one(fb,color_tex[0], color_tex[1], depth_rb, _width, _height) ;
	ClearFrameBuffers();

	return fbo_one;
}

void RenderScene::ApplySingleFrameBuffer(FBO& _fbo)
{
 	glBindFramebuffer(GL_FRAMEBUFFER, _fbo.m_frameBufferIndex);
	glViewport(0,0,_fbo.m_width,_fbo.m_height);
TESTGL;
 	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fbo.m_renderTextureIndex, 0);
TESTGL;
 	// Set the list of draw buffers.
 	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
 	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
TESTGL;
 	glBindFramebuffer(GL_FRAMEBUFFER, _fbo.m_frameBufferIndex);
	glViewport(0,0,_fbo.m_width,_fbo.m_height);
TESTGL;
}

void RenderScene::ApplyTwoFrameBuffers(FBOMulti _fbo)
{
 	glBindFramebuffer(GL_FRAMEBUFFER, _fbo.m_frameBufferIndex);
	glViewport(0,0,_fbo.m_width,_fbo.m_height);

 	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _fbo.m_renderTextureIndex0, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _fbo.m_renderTextureIndex1, 0);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, _fbo.m_renderTextureIndex2, 0);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, _fbo.m_renderTextureIndex3, 0);

 	// Set the list of draw buffers.
 	//GLenum DrawBuffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
 	GLenum DrawBuffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1}; 	
	glDrawBuffers(4, DrawBuffers);


}

void RenderScene::ClearFrameBuffers()
{
	// Render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport( 0, 0, m_SizeX, m_SizeY);


TESTGL;
 	//GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
 	//glDrawBuffers(1, DrawBuffers);

TESTGL;
}










