#include "RenderScene.h"
#include <iostream>

void RenderScene::TestGLError(const char* _file, int _line)
{
	GLenum id = glGetError();

	if(GL_NO_ERROR == id)
		return;
	else
		std::cerr << "OpenGL Error: (" << _file << ", " << _line << ")-> " << id << "\n";
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



void RenderScene::InitialiseRenderObjects()
{
	m_genericUnitQuad.Initialise();

	FBO newFBO = CreateSingleFrameBuffer(1024,1024,0);
	FBOMulti newFBOMulti = CreateFourFrameBuffer(1024,1024,0);

	m_FBOList.push_back(newFBO);
	m_FBOMultiList.push_back(newFBOMulti);

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

void RenderScene::Render_Scene()
{
		glUseProgram(m_shaderMap.find(std::string("TestShader_MRT"))->second.programID);
		ApplyFourFrameBuffers(m_FBOMultiList[0]);

	    glClearColor(0.0, 1.0, 0.0, 0.0);
	    glClearDepth(1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set Uniform Values
		m_shaderMap.find(std::string("TestShader_MRT"))->second.SetUniform1F("testVal", 1.0f);
		m_genericUnitQuad.Draw();

		ClearFrameBuffers();
}

void RenderScene::Render_CopyToViews()
{
	glUseProgram(m_shaderMap.find(std::string("TestShader_Tex"))->second.programID);

	m_shaderMap.find(std::string("TestShader_Tex"))->second.SetUniform1UI("tex", 0);
	m_shaderMap.find(std::string("TestShader_Tex"))->second.SetUniform1F("testVal", 1.0f);

/*
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_FBOMultiList[0].m_renderTextureIndex0);
	Position p0(-0.5f, 0.f, 0.f);
	size 	 s0(0.25f, 0.25f);
	RenderScreenQuadAtOffset(p0,s0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_FBOMultiList[0].m_renderTextureIndex1);
	Position p1(0.5f, 0.5f, 0.f);
	size 	 s1(0.25f, 0.25f);
	RenderScreenQuadAtOffset(p1,s1);*/

	GLuint textureArray[4] =  {	m_FBOMultiList[0].m_renderTextureIndex0,
								m_FBOMultiList[0].m_renderTextureIndex1,
								m_FBOMultiList[0].m_renderTextureIndex2,
								m_FBOMultiList[0].m_renderTextureIndex3
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
    glClear(GL_COLOR_BUFFER_BIT);

    //glCullFace(GL_NONE);
	glDisable( GL_CULL_FACE );
	glDisable( GL_DEPTH_TEST );
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












