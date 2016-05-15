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

bool RenderScene::LoadMaterial( std::string _file, std::string _name)
{
	std::ifstream file;
	file.open(_file.c_str(), std::ios::in);
	if(!file)
	{ 
		std::cerr << "\t Failed to open screen config file: " << _file << "\n";
		return false;
	}


	char fileTypeMask[9] = { 'v', 'v', 'v', 'v', 'f', 'f', 't', 't', 't'};
    colour colArray[4];
	int colIndex = 0;

    float floatArray[4];
	int floatIndex = 0;

    Texture texArray[3];
	int texIndex = 0;


	int index = 0;
	
	std::string line;
	if (file.is_open())
	{
		std::cerr << "Reading Config file>\n";
		while ( getline (file,line) )
		{
			if(fileTypeMask[index] == 'v')
			{
				std::vector<std::string> sData = split(line, ',');
				if (sData.size() == 4 )
				{
					float r = (float)atof(sData[0].c_str());
					float g = (float)atof(sData[1].c_str());
					float b = (float)atof(sData[2].c_str());
					float a = (float)atof(sData[3].c_str());

					std::cerr << "Col Index: " << colIndex << " Values: " << r << ", " << g << ", " << b << ", " << a << "\n";

					colour col(r,g,b,a);
					colArray[colIndex] = col;
					colIndex++;
					
				}
			}	
			else if(fileTypeMask[index] == 'f')
			{
				float f = (float)atof(line.c_str());

				std::cerr << "Float Index: " << floatIndex << " Value: " << f << "\n";

				floatArray[floatIndex] = f;
				floatIndex++;
			}
			else if(fileTypeMask[index] == 't')
			{
				Texture t;
				t.Init(line);
				std::cerr << "Tex Index: " << texIndex << " Value: " << line << "\n";

				texArray[texIndex] = t;
				texIndex++;
			}

			index++;
		}

		Material mat(_name, colArray[0], colArray[1], colArray[2], colArray[3], floatArray[0], floatArray[1],
						texArray[0], texArray[1], texArray[2] );

		
		m_materialMap.insert(std::make_pair(_name,  mat)  );

		return true;
	}

	return false;
}


void ScreenWriteToFile	(std::string _path, RenderScene* _RS)
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

			Position 	pos 	= _RS[i].m_layoutControl.GetScreenPos(0);
			size		size 	= _RS[i].m_layoutControl.GetScreenSize(0);
			
			ssP << _RS[i].m_id << ":" << "P" << ":" << pos.X()  << ":" << pos.Y() << ":" << pos.Z() << ":0\n";
			ssP << _RS[i].m_id << ":" << "S" << ":" << size.W() << ":" << size.H() << ":0"<< ":0\n";

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
		std::cerr << "Reading Config file>\n";
		while ( getline (file,line) )
		{
			std::vector<std::string> sData = split(line, ':');
			//std::cerr << line << "\n";
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


				if(index == m_id)
				{
					//std::cerr << "Type is: " << type << " \n";
					if(type == 'P')
					{	
						Position p(val0, val1, val2);
						_layout->SetScreenPos(0, p);
					}
					else if(type == 'S')
					{	
						size s(val0, val1);
						_layout->SetScreenSize(0, s);
					}

				}

	
			}

			//std::cerr << "-";	
		}
		//std::cerr << "\n";
		//std::cerr << "\t Closing file.\n";
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


	std::string path = "../Mesh/sibenik.obj";
	AIMesh sibenik;

	glm::mat4 translate = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 48.f, 0.f));
	glm::mat4 model 	= translate * glm::scale(glm::mat4(1.0f), glm::vec3(4.4, 4.4, 4.4));


	sibenik.Initialise(path);
	sibenik.SetName("Sibenik");
    sibenik.SetMaterial("Test");
	sibenik.SetModelMat(model);

	m_meshArray.push_back(sibenik);

	path = "../Mesh/cubet.obj";
	AIMesh box;
	box.Initialise(path);
	box.SetMaterial("Box");
	box.SetName("box_0");
	
	translate = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 10.f));
	model 	  = translate * glm::scale(glm::mat4(1.0f), glm::vec3(4.4, 4.4, 4.4));
	box.SetModelMat(model);
	m_meshArray.push_back(box);

	box.SetName("box_1");
	translate = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 5.f));
	model 	  = translate * glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0));
	box.SetModelMat(model);
	m_meshArray.push_back(box);

	box.SetName("box_2");
	translate = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 40.f));
	model 	  = translate * glm::scale(glm::mat4(1.0f), glm::vec3(8.4, 8.4, 8.4));
	box.SetModelMat(model);
	m_meshArray.push_back(box);

	std::string img = "../Images/testimage.png";
	m_testTexture.Init(img);

	m_genericUnitQuad.Initialise();

	FBO 	 newFBO 	  = CreateSingleFrameBuffer	(1024,1024,0);
	FBOMulti newFBOMulti0 = CreateTwoFrameBuffer	(1024,1024,0);
	FBOMulti newFBOMulti1 = CreateTwoFrameBuffer	(1024,1024,0);

	m_FBOList.push_back(newFBO);
	m_FBOMultiList.push_back(newFBOMulti0);
	m_FBOMultiList.push_back(newFBOMulti1);

	//m_camera[0].Init( glm::vec3(-30.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f),
	//					50.f, 1.0f, 0.01f, 200.f); 

	
	//other
	//m_camera[1].Init( glm::vec3(0.f, 0.f, 30.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f),
	//					50.f, 1.0f, 0.01f, 200.f); 
}

void RenderScene::SetCamera(Camera& _cam)
{
	m_camera = _cam;
}

void RenderScene::SetLeftRight(bool _left)
{
	m_viewState.m_IsLeft = _left;
}

void RenderScene::Initialise()
{
	m_PosX = 0;
	m_PosY = 0;

	m_SizeX = 2048;
	m_SizeY = 1536;

	glViewport(0,0,1,1);

	InitialiseScreenPositions();
}

void RenderScene::SceneBody(ShaderProgram& _prog)
{
		glEnable(GL_DEPTH_TEST);
	    glClearColor(0.0, 0.0, 0.0, 0.0);
	    glClearDepth(1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*Add Nice Scene here */

		m_materialMap.find("Test")->second.Apply(_prog);

		float angle  = 0.00f;
		glm::mat4 rotation	= glm::rotate(glm::mat4(1.f), (glm::mediump_float)angle, glm::vec3(0,1,0));  

		for(unsigned int i = 0; i < m_meshArray.size(); i++)
		{
			m_materialMap.find(m_meshArray[i].GetMaterial())->second.Apply(_prog);	
	
			glm::mat4 model = m_meshArray[i].GetModelMat();

			_prog.SetMatrix4FV(std::string("mvp"), glm::value_ptr(m_camera.GetMVP(model)));
			_prog.SetMatrix4FV("m", glm::value_ptr(model));
			_prog.SetMatrix4FV("r", glm::value_ptr(rotation));

			m_meshArray[i].Draw();
		}

		//m_boxMesh.Draw();
}

void RenderScene::Render_Scene()
{
		static float angle = 0.00f;
		glm::mat4 rotation	= glm::rotate(glm::mat4(1.f), (glm::mediump_float)angle, glm::vec3(0,1,0));  
		m_camera.Rotate(rotation);


		std::string shadername = "";
		shadername = "TestShader_persp_MRT_near";
		if(m_viewState.m_IsNear)
		{
			m_shaderMap.find(shadername)->second.SetUniform1F("near"		, 1.f);
			m_shaderMap.find(shadername)->second.SetUniform1F("nearClip"	, 1.f);
			m_shaderMap.find(shadername)->second.SetUniform1F("farClip"		, 10.f);
			m_shaderMap.find(shadername)->second.SetUniform1F("blendClip"	, 15.f);

		}
		else
		{
			m_shaderMap.find(shadername)->second.SetUniform1F("near"		, 0.f);
			m_shaderMap.find(shadername)->second.SetUniform1F("nearClip"	, 10.f);
			m_shaderMap.find(shadername)->second.SetUniform1F("farClip"		, 9999.f);
			m_shaderMap.find(shadername)->second.SetUniform1F("blendClip"	, 15.f);
		}

		glUseProgram(m_shaderMap.find(shadername)->second.programID);

		ApplySingleFrameBuffer(m_FBOList[0]);

		//Apply light matrix
		colour lightColSize[8];
		for(unsigned int i = 0; i < 8; i++)
		{
			lightColSize[i].r = m_pointLights[i].m_colour.r;
			lightColSize[i].g = m_pointLights[i].m_colour.g;
			lightColSize[i].b = m_pointLights[i].m_colour.b;
			lightColSize[i].a = m_pointLights[i].m_distance;
		}
		colour lightPos[8];
		for(unsigned int i = 0; i < 8; i++)
		{
			lightPos[i].r = m_pointLights[i].m_pos.pos[0];
			lightPos[i].g = m_pointLights[i].m_pos.pos[1];
			lightPos[i].b = m_pointLights[i].m_pos.pos[2];
			lightPos[i].a = 1.0f;
		}
TESTGL;
		m_shaderMap.find(shadername)->second.SetUniform4FP("lightPosArray", (float*)&lightPos[0], 8);
		m_shaderMap.find(shadername)->second.SetUniform4FP("lightColScaleArray", (float*)&lightColSize[0], 8);

TESTGL;
		//m_shaderMap.find(shadername)->second.SetMatrix4FV("v", glm::value_ptr(m_camera.GetV()));
		//m_shaderMap.find(shadername)->second.SetMatrix4FV("p", glm::value_ptr(m_camera.GetP()));
		m_shaderMap.find(shadername)->second.SetUniform3FP("cameraPos", glm::value_ptr(m_camera.GetPos()));
		m_shaderMap.find(shadername)->second.SetUniform3FP("cameraDir", glm::value_ptr( glm::normalize( m_camera.GetDir() ) ) );

		
		SceneBody(m_shaderMap.find(shadername)->second);

		ClearFrameBuffers();
}

void RenderScene::Render_CopyToViews()
{
	glUseProgram(m_shaderMap.find(std::string("TestShader_Tex"))->second.programID);

	m_shaderMap.find(std::string("TestShader_Tex"))->second.SetUniform1UI("tex", 0);
	m_shaderMap.find(std::string("TestShader_Tex"))->second.SetUniform1F("testVal", 1.0f);
	m_shaderMap.find(std::string("TestShader_Tex"))->second.SetMatrix4FV("XYZtoRGB", glm::value_ptr(m_XYZtoRGB));

	GLuint textureArray[1] =  {	m_FBOList[0].m_renderTextureIndex };

	for(unsigned int i = 0; i < 1; i++)
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

void RenderScene::IncrementMode()
{
	m_renderMode = (ERENDERMODE)(int( m_renderMode + 1) % (E_END) );
}

void RenderScene::Render()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	
	switch (m_renderMode)
	{
		case E_MODELSCENE:
		{
			glCullFace(GL_BACK);
			{
				Render_Scene();
				Render_CopyToViews();
			}
		}
		break;

		case E_RED:
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport( 0, 0, m_SizeX, m_SizeY);
		    glClearColor(1.0, 0.0, 0.0, 0.0);
		    glClearDepth(1.0f);
		    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		break;

		case E_GREEN:
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport( 0, 0, m_SizeX, m_SizeY);
		    glClearColor(0.0, 1.0, 0.0, 0.0);
		    glClearDepth(1.0f);
		    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);;
		}
		break;

		case E_BLUE:
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport( 0, 0, m_SizeX, m_SizeY);
		    glClearColor(0.0, 0.0, 1.0, 0.0);
		    glClearDepth(1.0f);
		    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		break;

		case E_BLANK:
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport( 0, 0, m_SizeX, m_SizeY);
		    glClearColor(0.0, 0.0, 0.0, 0.0);
		    glClearDepth(1.0f);
		    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		break;

		default:
			std::cerr << "Defualt mode render mode\n";
			break;

	}
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

	//Camera Translation
	if( _key == 't' )
	{
		m_camera.Translate( 1.f, 0.f, 0.f);
	}
	else if( _key == 'g' )
	{
		m_camera.Translate( -1.f, 0.f, 0.f);
	}
	else if( _key == 'f' )
	{
		m_camera.Translate( 0.f, 0.f, -1.f);
	}
	else if( _key == 'h' )
	{
		m_camera.Translate( 0.f, 0.f, 1.f);
	}

	else if( _key == 'p')
	{
		m_camera.ChangeFOV(0.01f);
	}
	else if( _key == 'o')
	{
		m_camera.ChangeFOV(-0.01f);
	}

	else if	( _key == 'w' )
		m_layoutControl.AdjustScreenPos(Position(0.f, 0.01f, 0.f), 0);
	else if	( _key == 's' )
		m_layoutControl.AdjustScreenPos(Position(0.f, -0.01f, 0.f), 0);
	else if ( _key == 'a' )
		m_layoutControl.AdjustScreenPos(Position(-0.01f, 0.0f, 0.f), 0);
	else if ( _key == 'd' )
		m_layoutControl.AdjustScreenPos(Position(0.01f, 0.0f, 0.f), 0);

	else if ( _key == 'e' )
		m_layoutControl.AdjustScreenSize(size(0.01f, 0.01f), 0);
	else if ( _key == 'q' )
		m_layoutControl.AdjustScreenSize(size(-0.01f, -0.01f), 0);

	else if ( _key == 'm')
	{
			IncrementMode();
	}
}

void RenderScene::SetLight( glm::vec3 _pos, glm::vec3 _colour, float _scale, int _index)
{
	m_pointLights[_index].Set(_pos, _colour, _scale);
}

void RenderScene::SetXYZtoRGBMat( glm::mat4& _mat)
{
	m_XYZtoRGB = _mat;
}













