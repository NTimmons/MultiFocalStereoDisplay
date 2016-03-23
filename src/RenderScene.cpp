#include "RenderScene.h"
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <sys/time.h>

unsigned long GetTickCount()
{
	struct timeval tv;
	if( gettimeofday(&tv, NULL) != 0 )
    	return 0;

	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

void RenderScene::TestGLError(const char* _file, int _line)
{
	GLenum id = glGetError();

	if(GL_NO_ERROR == id)
		return;
	else
		std::cerr << "OpenGL Error: (" << _file << ", " << _line << ")-> " << id << "\n";
}

void RenderScene::InitialiseRenderObjects()
{
	std::string path = "path";
	m_boxMesh.Initialise(path);

	std::string img = "../Images/testimage.png";
	m_testTexture.Init(img);

	m_genericUnitQuad.Initialise();

	FBO newFBO = CreateSingleFrameBuffer(1024,1024,0);
	FBOMulti newFBOMulti0 = CreateTwoFrameBuffer(1024,1024,0);
	FBOMulti newFBOMulti1 = CreateTwoFrameBuffer(1024,1024,0);

	m_FBOList.push_back(newFBO);
	m_FBOMultiList.push_back(newFBOMulti0);
	m_FBOMultiList.push_back(newFBOMulti1);

	m_camera[0].Init( glm::vec3(-1.f, 10.f, -30.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f),
					50.f, 1.0f, 0.01f, 200.f); 

	m_camera[1].Init( glm::vec3(1.f, 10.f, -30.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f),
					50.f, 1.0f, 0.01f, 200.f); 
}

void RenderScene::Initialise()
{
	m_PosX = 0;
	m_PosY = 0;

	m_SizeX = 800;
	m_SizeY = 800;

	glViewport(0,0,1,1);

	InitialiseScreenPositions();
}


void RenderScene::SceneBody()
{
		glEnable(GL_DEPTH_TEST);
	    glClearColor(0.6, 0.2, 0.4, 0.0);
	    glClearDepth(1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*Add Nice Scene here */
		m_boxMesh.Draw();
}

void RenderScene::Render_Scene()
{
		static float angle = 0.f;
		angle+=0.06f;

		//Model setup.
		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(2.0, 2.0, 2.0));
		model =  glm::rotate(model, (glm::mediump_float)angle, glm::vec3(0,1,0));      //glm::rotate(model, 60.f, glm::vec3(0.0, 1.0, 0.0));

		//Use shader
		glUseProgram(m_shaderMap.find(std::string("TestShader_persp_MRT"))->second.programID);

		//Render left
		ApplyTwoFrameBuffers(m_FBOMultiList[0]);
		m_shaderMap.find(std::string("TestShader_persp_MRT"))->second.SetMatrix4FV("mvp", glm::value_ptr(m_camera[0].GetMVP(model)));
		SceneBody();

		//Render right
		ApplyTwoFrameBuffers(m_FBOMultiList[1]);
		m_shaderMap.find(std::string("TestShader_persp_MRT"))->second.SetMatrix4FV("mvp", glm::value_ptr(m_camera[1].GetMVP(model)));
		SceneBody();

		ClearFrameBuffers();
}

void RenderScene::Render_CopyToViews()
{
	glUseProgram(m_shaderMap.find(std::string("TestShader_Tex"))->second.programID);

	m_shaderMap.find(std::string("TestShader_Tex"))->second.SetUniform1UI("tex", 0);
	m_shaderMap.find(std::string("TestShader_Tex"))->second.SetUniform1F("testVal", 1.0f);

	GLuint textureArray[4] =  {	m_FBOMultiList[0].m_renderTextureIndex0,
								m_FBOMultiList[0].m_renderTextureIndex1,
								m_FBOMultiList[1].m_renderTextureIndex0,
								m_testTexture.Get()//m_FBOMultiList[1].m_renderTextureIndex1
							  };

	for(unsigned int i = 0; i < 4; i++)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureArray[i]);
		Position p = m_layoutControl.GetScreenPos(i);
		size 	 s = m_layoutControl.GetScreenSize(i);
		RenderScreenQuadAtOffset(p,s);
	}

	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderScene::Render()
{
    glClearColor(0.3, 0.4, 0.8, 0.0);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_BACK);
    {
		Render_Scene();
		Render_CopyToViews();
    }

    glFlush();
    glutSwapBuffers() ;
}

void RenderScene::RenderScreenQuadAtOffset(Position& _offset, size& _size)
{
	m_shaderMap.find(std::string("TestShader_Tex"))->second.SetUniform4F("positionOffset", _offset.X(), _offset.Y(), _offset.Z(), 0.f);
	m_shaderMap.find(std::string("TestShader_Tex"))->second.SetUniform2F("scale", _size.W(), _size.H());
	m_genericUnitQuad.Draw();

}

void RenderScene::InitialiseScreenPositions()
{
/*
	m_screenArray[0].m_pos = Position(0.f     , 100.f, 0.f); 
	m_screenArray[1].m_pos = Position(200.f   , 100.f, 0.f);
	m_screenArray[2].m_pos = Position(400.f   , 100.f, 0.f);
	m_screenArray[3].m_pos = Position(600.f   , 100.f, 0.f);

	m_screenArray[0].m_size = size(100.f, 100.0f);
	m_screenArray[1].m_size = size(100.f, 100.0f);
	m_screenArray[2].m_size = size(100.f, 100.0f);
	m_screenArray[3].m_size = size(100.f, 100.0f);

	m_screenArray[0].m_colour = colour(1.f, 0.f, 0.f, 1.f);
	m_screenArray[1].m_colour = colour(0.f, 1.f, 0.f, 1.f);
	m_screenArray[2].m_colour = colour(0.f, 0.f, 1.f, 1.f);
	m_screenArray[3].m_colour = colour(1.f, 1.f, 0.f, 1.f);
*/
}

void RenderScene::RenderQuad(Position& _pos, size& _size, colour& _col)
{
	glColor3f(_col.R(), _col.G(), _col.B());
            glVertex3f(_pos.X()				, _pos.Y()				, 0);
            glVertex3f(_pos.X()				,  _pos.Y() + _size.H()	, 0);
            glVertex3f( _pos.X()+ _size.W()	,  _pos.Y() + _size.H()	, 0);

            glVertex3f(_pos.X()				, _pos.Y()				, 0);
            glVertex3f(_pos.X()+ _size.W()	, _pos.Y() + _size.H()	, 0);
            glVertex3f(_pos.X()+ _size.W()  , _pos.Y()				, 0);
}

void RenderScene::HandleInput( unsigned char _key)
{

	//Which screen to move;
	if		( _key == '0')
		m_activeScreen = 0;
	else if	( _key == '1' )
		m_activeScreen = 1;
	else if ( _key == '2' )
		m_activeScreen = 2;
	else if ( _key == '3' )
		m_activeScreen = 3;

	else if	( _key == 'w' )
		m_layoutControl.AdjustScreenPos(Position(0.f, 0.01f, 0.f), m_activeScreen);
	else if	( _key == 's' )
		m_layoutControl.AdjustScreenPos(Position(0.f, -0.01f, 0.f), m_activeScreen);
	else if ( _key == 'a' )
		m_layoutControl.AdjustScreenPos(Position(-0.01f, 0.0f, 0.f), m_activeScreen);
	else if ( _key == 'd' )
		m_layoutControl.AdjustScreenPos(Position(0.01f, 0.0f, 0.f), m_activeScreen);

	else if ( _key == 'e' )
		m_layoutControl.AdjustScreenSize(size(0.01f, 0.01f), m_activeScreen);
	else if ( _key == 'q' )
		m_layoutControl.AdjustScreenSize(size(-0.01f, -0.01f), m_activeScreen);

}












