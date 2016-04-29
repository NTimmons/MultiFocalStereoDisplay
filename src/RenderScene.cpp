#include "RenderScene.h"
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#ifndef _WIN32
#include <sys/time.h>
#endif

#include <sstream>


#ifndef _WIN32
unsigned long GetTickCount()
{
	struct timeval tv;
	if( gettimeofday(&tv, NULL) != 0 )
    	return 0;

	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
#endif

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) 
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) 
	{
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) 
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

//------------------------------------------------------------------------///



void RenderScene::TestGLError(const char* _file, int _line)
{
	GLenum id = glGetError();

	if(GL_NO_ERROR == id)
		return;
	else
		std::cerr << "OpenGL Error: (" << _file << ", " << _line << ")-> " << id << "\n";
}


void RenderScene::ScreenWriteToFile	(std::string _path, ScreenLayout* _screen)
{
	std::ofstream file;
	file.open(_path.c_str(), std::ios::out);
	if(!file)
	{ 
		std::cerr << "\t Failed to open screen config file: " << _path << "\n";
		return;
	}

	if (file.is_open())
	{
		for(unsigned int i = 0; i < 4; i++)
		{
			std::stringstream ssP;
			std::stringstream ssS;
			unsigned int index = i;	

			Position 	pos 	= _screen->GetScreenPos(i);
			size		size 	= _screen->GetScreenSize(i);
			
			ssP << index << ":" << "P" << ":" << pos.X() << ":" << pos.Y() << ":" << pos.Z() << ":0\n";
			ssP << index << ":" << "S" << ":" << size.W() << ":" << size.H() << ":0"<< ":0\n";

			file << ssP.str();
			file << ssS.str();

		}

		file.close();
	}	
}

bool RenderScene::ScreenLoadFromFile(std::string _path, ScreenLayout* _layout)
{
	std::ifstream file;
	file.open(_path.c_str(), std::ios::in);
	if(!file)
	{ 
		std::cerr << "\t Failed to open screen config file: " << _path << "\n";
		return false;
	}
	
	std::string line;
	if (file.is_open())
	{
		std::cerr << "\t Reading Config file>\n";
		while ( getline (file,line) )
		{
			std::vector<std::string> sData = split(line, ':');
			std::cerr << line << "\n";
			if(sData.size() != 0 && sData.size() != 6)
			{
				std::cerr << "Invalid Config File\n";
			}
			else if (sData.size() > 0 )
			{

				unsigned int 	index 	= atoi(sData[0].c_str());
				unsigned char 	type 	= sData[1].c_str()[0];
				float 			val0	= (float)atof(sData[2].c_str());
				float 			val1	= (float)atof(sData[3].c_str());
				float 			val2	= (float)atof(sData[4].c_str());
				float 			val3	= (float)atof(sData[5].c_str());

				/*
				std::cerr << "Index: " << index <<
							 " type: " << type <<
							 " val0: " << val0 <<
							 " val1: " << val1 <<
							 " val2: " << val2 <<
							 " val3: " << val3 << "\n";
				*/

				//std::cerr << "Type is: " << type << " \n";
				if(type == 'P')
				{	
					Position p(val0, val1, val2);
					_layout->SetScreenPos(index, p);
				}
				else if(type == 'S')
				{	
					size s(val0, val1);
					_layout->SetScreenSize(index, s);
				}
			}

			//std::cerr << "-";	
		}
		std::cerr << "\n";
		std::cerr << "\t Closing file.\n";
		file.close();
		return true;

	}

	return false;
}


void RenderScene::InitialiseRenderObjects()
{
	//Load Screen Data
	std::string configFile = "../Config/layout.data";
	ScreenLoadFromFile(configFile, &m_layoutControl);


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

	m_camera[0].Init( glm::vec3(-30.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f),
					50.f, 1.0f, 0.01f, 200.f); 

	m_camera[1].Init( glm::vec3(0.f, 0.f, 30.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f),
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
								m_FBOMultiList[1].m_renderTextureIndex1
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

	else if ( _key == 'p')
	{
			std::string configFile = "../Config/layout.data";
			ScreenWriteToFile(configFile, &m_layoutControl);
	}
}












