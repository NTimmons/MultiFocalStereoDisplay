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

void RenderScene::SetBlendMode(float _blendMode)
{
	m_blendMode = _blendMode;
}

void RenderScene::SetScene( int _sceneID)
{
	m_sceneID = _sceneID;
}

void RenderScene::IncrementBlendMode()
{
	m_blendMode += 1.0f;
	if (m_blendMode > 5.0f)
	{
		m_blendMode = 0.f;
	}

	std::cerr << "BlendMode = " << m_blendMode << "\n";
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

	char 	fileTypeMask[9] = { 'v', 'v', 'v', 'v', 'f', 'f', 't', 't', 't'};
    colour 	colArray[4];
	int 	colIndex = 0;

    float 	floatArray[4];
	int 	floatIndex = 0;

    Texture texArray[3];
	int 	texIndex = 0;

	int		index = 0;
	
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

				if( (int)index == m_id)
				{
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
		}
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

	sibenik.Initialise(path);
	sibenik.SetName("Sibenik");
    sibenik.SetMaterial("Test");
	sibenik.SetScaleMat(glm::scale(glm::mat4(1.0f), glm::vec3(4.4, 4.4, 4.4)));
	sibenik.SetTranslationMat( glm::translate(glm::mat4(1.f), glm::vec3(-0.f, 38.f, 0.f) ) );
	m_meshArray.push_back(sibenik);

	path = "../Mesh/teapot.obj";
	AIMesh teapot;
	teapot.Initialise(path);
	teapot.SetMaterial("Test");
	teapot.SetName("Teapot");
	teapot.SetScaleMat(glm::mat4(1.f));
	teapot.SetRotationMat(glm::mat4(1.f));
	teapot.SetTranslationMat(glm::translate(glm::mat4(1.f), glm::vec3(0.f, 3.f, 5.f)));

	path = "../Mesh/teapot.obj";
	AIMesh teapotRot;
	teapotRot.Initialise(path);
	teapotRot.SetMaterial("Test");
	teapotRot.SetName("Teapot_Rotation_0");
	teapotRot.SetScaleMat(glm::mat4(1.f));
	teapotRot.SetRotationMat(glm::mat4(1.f));
	teapotRot.SetTranslationMat(glm::translate(glm::mat4(1.f), glm::vec3(0.f, 3.f, 5.f)));
	m_meshArray.push_back(teapotRot);

	path = "../Mesh/groundPlane.obj";
	AIMesh plane;
	plane.Initialise(path);
	plane.SetMaterial("Test");
	plane.SetName("Plane");
	translate = glm::translate(glm::mat4(1.f), glm::vec3(-0.5f, 0.f, -0.5f));
	plane.SetScaleMat(glm::scale(glm::mat4(1.0f), glm::vec3(100.f, 1.f, 100.f)));
	plane.SetRotationMat(glm::mat4(1.f));
	plane.SetTranslationMat(translate);
	m_meshArray.push_back(plane);

	path = "../Mesh/blob.obj";
	AIMesh blob;
	blob.Initialise(path);

	path = "../Mesh/cubet.obj";
	AIMesh box;
	box.Initialise(path);
	box.SetMaterial("Box");

	box.SetScaleMat			(glm::mat4(1.f));
	box.SetRotationMat		(glm::mat4(1.f));
	box.SetTranslationMat	(glm::mat4(1.f));
	
	blob.SetName("Box_Left_Distance_0");
	blob.SetMaterial("Red");
	blob.SetRotationMat( glm::rotate(glm::mat4(1.f), (glm::mediump_float)1.0f, glm::vec3(0,1,0))); 
	m_meshArray.push_back(blob);
	blob.SetName("Box_Middle_Distance_0");
	blob.SetMaterial("Blue");
	blob.SetRotationMat( glm::rotate(glm::mat4(1.f), (glm::mediump_float)1.0f, glm::vec3(1,1,0)) ); 
	m_meshArray.push_back(blob);
	blob.SetName("Box_Far_Distance_0");
	blob.SetMaterial("Green");
	blob.SetRotationMat( glm::rotate(glm::mat4(1.f), (glm::mediump_float)1.0f, glm::vec3(0,1,1) )); 
	m_meshArray.push_back(blob);

	blob.SetName("Box_Translation_0");
	blob.SetMaterial("Green");
	m_meshArray.push_back(blob);
	
	box.SetName("Box_NEAR_0");
	box.SetTranslationMat	(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 5.4f + 0.5f)) );
	m_meshArray.push_back(box);

	box.SetName("Box_FAR_0");
	box.SetTranslationMat	(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 8.1f  + 0.5f)) );
	m_meshArray.push_back(box);

	//Box grid
	for(int x = 0; x < 10; x++)
	{
		for(int y = 0; y < 10; y++)
		{
			std::stringstream boxStr;
			boxStr << "box_";
			boxStr << x << "_" << y; 
			box.SetName(boxStr.str().c_str());
			translate = glm::translate(glm::mat4(1.f), glm::vec3((x-5) * 2.0f, -2.5f, (y-5) * 2.f));
			box.SetScaleMat(glm::mat4(1.f));
			box.SetRotationMat(glm::mat4(1.f));
			box.SetTranslationMat(translate);
			m_meshArray.push_back(box);
		}
	}

	std::string img = "../Images/testimage.png";
	m_testTexture.Init(img);

	img = "../Images/stop.png";
	m_stop.Init(img);

	img = "../Images/nearCalib.png";
	m_nearCalibration.Init(img);

	img = "../Images/farCalib.png";
	m_farCalibration.Init(img);

	img = "../Images/decision.png";	
	m_decision.Init(img);

	m_genericUnitQuad.Initialise();

	FBO 	 newFBO 	  = CreateSingleFrameBuffer	(1024,1024,0);
	FBOMulti newFBOMulti0 = CreateTwoFrameBuffer	(1024,1024,0);
	FBOMulti newFBOMulti1 = CreateTwoFrameBuffer	(1024,1024,0);

	m_FBOList.push_back(newFBO);
	m_FBOMultiList.push_back(newFBOMulti0);
	m_FBOMultiList.push_back(newFBOMulti1);
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

void RenderScene::SceneBody_Test(ShaderProgram& _prog)
{
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
	    glClearColor(0.0, 0.0, 0.0, 0.0);
	    glClearDepth(1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*Add Nice Scene here */
		m_materialMap.find("Test")->second.Apply(_prog);

		//static float angle  = 0.00f;
		//angle += 0.01f;
		//glm::mat4 rotation	= glm::rotate(glm::mat4(1.f), (glm::mediump_float)angle, glm::vec3(0,1,0));  

		//Rotate the teapot.
		//AIMesh* pot = GetMesh("Teapot");
		//pot->SetRotationMat(rotation);

		for(unsigned int i = 0; i < m_meshArray.size(); i++)
		{
			m_materialMap.find(m_meshArray[i].GetMaterial())->second.Apply(_prog);	
	
			glm::mat4 model = m_meshArray[i].GetModelMat();

			_prog.SetMatrix4FV(std::string("mvp"), glm::value_ptr(m_camera.GetMVP(model)));
			_prog.SetMatrix4FV("m", glm::value_ptr(model));
			_prog.SetMatrix4FV("r", glm::value_ptr(m_meshArray[i].GetRotationMat()));

			m_meshArray[i].Draw();
		}

}

void RenderScene::SceneBody_Rotation(ShaderProgram& _prog, float _depth, float _speed)
{
	(void)_depth;

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static float angle  = 1.5708f * 0.5f;
	angle+=_speed;
	for(int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			//float angle  = 1.5708f * 0.5f;
			glm::mat4 rotation		= glm::rotate(glm::mat4(1.f)	, (glm::mediump_float)angle, glm::vec3(0,1,0));  
			glm::mat4 translation 	= glm::translate(glm::mat4(1.f)	, glm::vec3( (x-2) * 3.f, -1.625f, (y+1) * (3.5f) ));
			glm::mat4 scale			= glm::scale(glm::mat4(1.0f)	, glm::vec3(0.50f, 0.50f, 0.50f));

			//Rotate the teapot.
			AIMesh* pot = GetMesh("Teapot_Rotation_0");
			pot->SetScaleMat(scale);
			pot->SetRotationMat(rotation);
			pot->SetTranslationMat(translation);

			m_materialMap.find(pot->GetMaterial())->second.Apply(_prog);	

			glm::mat4 model = pot->GetModelMat();
			_prog.SetMatrix4FV(std::string("mvp"), glm::value_ptr(m_camera.GetMVP(model)));
			_prog.SetMatrix4FV("m", glm::value_ptr(model));
			_prog.SetMatrix4FV("r", glm::value_ptr(pot->GetRotationMat()));
			pot->Draw();
		}

	}
}

void RenderScene::SceneBody_Translation(ShaderProgram& _prog, float _depthMin, float _depthMax, float _speed)
{
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	AIMesh* box = GetMesh("Box_Translation_0");
	m_materialMap.find(box->GetMaterial())->second.Apply(_prog);	
		
	static float angle  = 0.00f;
	angle += _speed;

	float alpha = 0.5f + sin(angle)/2;
	float alpha1 = 0.5f + sin(angle + 1.5708f)/2;
	float alpha2 = 0.5f + sin((angle/2) + (1.5708f * 2.0f))/2;


	float depth = (alpha * _depthMin) + ( (1.0f - alpha) * _depthMax);
	glm::mat4 translateLeft = glm::translate(glm::mat4(1.f), glm::vec3(-1.4f, 0.f, depth));//
	box->SetTranslationMat(translateLeft);
	glm::mat4 model = box->GetModelMat();
	_prog.SetMatrix4FV(std::string("mvp"), glm::value_ptr(m_camera.GetMVP(model)));
	_prog.SetMatrix4FV("m", glm::value_ptr(model));
	_prog.SetMatrix4FV("r", glm::value_ptr(box->GetRotationMat()));
	box->Draw();

	depth = (alpha1 * _depthMin) + ( (1.0f - alpha1) * _depthMax);
	translateLeft = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, depth));//
	box->SetTranslationMat(translateLeft);
	model = box->GetModelMat();
	_prog.SetMatrix4FV(std::string("mvp"), glm::value_ptr(m_camera.GetMVP(model)));
	_prog.SetMatrix4FV("m", glm::value_ptr(model));
	_prog.SetMatrix4FV("r", glm::value_ptr(box->GetRotationMat()));
	box->Draw();

	depth = (alpha2 * _depthMin) + ( (1.0f - alpha2) * _depthMax);
	translateLeft = glm::translate(glm::mat4(1.f), glm::vec3(1.4f, 0.f, depth));//
	box->SetTranslationMat(translateLeft);
	model = box->GetModelMat();
	_prog.SetMatrix4FV(std::string("mvp"), glm::value_ptr(m_camera.GetMVP(model)));
	_prog.SetMatrix4FV("m", glm::value_ptr(model));
	_prog.SetMatrix4FV("r", glm::value_ptr(box->GetRotationMat()));
	box->Draw();
}

void RenderScene::SceneBody_Static(ShaderProgram& _prog)
{
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(int x = 0; x < 1; x++)
	{
		for (int y = 0; y < 1; y++)
		{
			for( int z = 0; z < 1; z++)
			{
				float angle  = 1.5708f * 0.5f;
				glm::mat4 rotation		= glm::rotate(glm::mat4(1.f)	, (glm::mediump_float)angle, glm::vec3(0,1,0));  
				//glm::mat4 translation 	= glm::translate(glm::mat4(1.f)	, glm::vec3( (x-4) * 2.1f,  -1.5625f + (z * 2.5f), (y+1) * (3.5f) ));
				glm::mat4 translation 	= glm::translate(glm::mat4(1.f)	, glm::vec3( (x) * 3.1f,  -0.5625f + (z * 2.5f), (y+1) * (7.0f) ));
				glm::mat4 scale			= glm::scale(glm::mat4(1.0f)	, glm::vec3(1.0f, 1.0f, 1.0f));

				//Rotate the teapot.
				AIMesh* pot = GetMesh("Teapot_Rotation_0");
				pot->SetScaleMat(scale);
				pot->SetRotationMat(rotation);
				pot->SetTranslationMat(translation);

				m_materialMap.find(pot->GetMaterial())->second.Apply(_prog);	

				glm::mat4 model = pot->GetModelMat();
				_prog.SetMatrix4FV(std::string("mvp"), glm::value_ptr(m_camera.GetMVP(model)));
				_prog.SetMatrix4FV("m", glm::value_ptr(model));
				_prog.SetMatrix4FV("r", glm::value_ptr(pot->GetRotationMat()));
				pot->Draw();
			}
		}
	}
}

void RenderScene::SceneBody_Distance(ShaderProgram& _prog, glm::vec3 _left, glm::vec3 _right)
{
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
	    glClearColor(0.0, 0.0, 0.0, 0.0);
	    glClearDepth(1.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		AIMesh* boxLeft 	= GetMesh("Box_Left_Distance_0");
		//AIMesh* boxMiddle 	= GetMesh("Box_Middle_Distance_0");
		AIMesh* boxRight 	= GetMesh("Box_Far_Distance_0");

		m_materialMap.find(boxLeft->GetMaterial())->second.Apply(_prog);	
		
		glm::mat4 scaleA = glm::scale(glm::mat4(1.f), glm::vec3(m_depthControls.scaleA, m_depthControls.scaleA, m_depthControls.scaleA) );
		glm::mat4 scaleB = glm::scale(glm::mat4(1.f), glm::vec3(m_depthControls.scaleB, m_depthControls.scaleB, m_depthControls.scaleB) );
		//glm::mat4 scaleC = glm::scale(glm::mat4(1.f), glm::vec3(m_depthControls.scaleC, m_depthControls.scaleC, m_depthControls.scaleC) );


		glm::mat4 translateLeft = glm::translate(glm::mat4(1.f), _left);//
		boxLeft->SetTranslationMat(translateLeft);
		boxLeft->SetRotationMat( glm::rotate(glm::mat4(1.f), (glm::mediump_float)m_depthControls.rotA, glm::vec3(0,1,0))); 
		boxLeft->SetScaleMat(glm::scale(scaleA, glm::vec3( _left.z/6.f )));

		glm::mat4 model = boxLeft->GetModelMat();
		_prog.SetMatrix4FV(std::string("mvp"), glm::value_ptr(m_camera.GetMVP(model)));
		_prog.SetMatrix4FV("m", glm::value_ptr(model));
		_prog.SetMatrix4FV("r", glm::value_ptr(boxLeft->GetRotationMat()));
		boxLeft->Draw();

		m_materialMap.find(boxLeft->GetMaterial())->second.Apply(_prog);	
		glm::mat4 translateRight = glm::translate(glm::mat4(1.f), _right);//glm::vec3( 1.0f, -0.f, 6.5f ));
		boxRight->SetTranslationMat(translateRight);
		boxRight->SetRotationMat( glm::rotate(glm::mat4(1.f), (glm::mediump_float)m_depthControls.rotB, glm::vec3(0,1,0))); 
		boxRight->SetScaleMat(glm::scale(scaleB, glm::vec3( _right.z/6.f )));

		model = boxRight->GetModelMat();
		_prog.SetMatrix4FV(std::string("mvp"), glm::value_ptr(m_camera.GetMVP(model)));
		_prog.SetMatrix4FV("m", glm::value_ptr(model));
		_prog.SetMatrix4FV("r", glm::value_ptr(boxRight->GetRotationMat()));
		boxRight->Draw();
}

void RenderScene::SceneBody_Calibration()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(50.0, 0.0, 50.0, 0.0);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);

	glUseProgram(m_shaderMap.find(std::string("TestShader_Tex_Calibration"))->second.programID);

	m_shaderMap.find(std::string("TestShader_Tex_Calibration"))->second.SetUniform1UI("tex", 0);
	m_shaderMap.find(std::string("TestShader_Tex_Calibration"))->second.SetUniform1F("testVal", 1.0f);

	GLuint textureArray;
	if(m_viewState.m_IsNear)
	{
		textureArray = m_nearCalibration.Get();
	}
	else
	{
		textureArray = m_farCalibration.Get() ;
	}

	ShaderProgram shad = m_shaderMap.find(std::string("TestShader_Tex_Calibration"))->second;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureArray);
	Position p (-1,-1,0);
	size 	 s (2,2);
	RenderScreenQuadAtOffset(shad, p,s);

	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void RenderScene::SceneBody_Stop()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(50.0, 0.0, 50.0, 0.0);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);

	glUseProgram(m_shaderMap.find(std::string("TestShader_Tex_Calibration"))->second.programID);

	m_shaderMap.find(std::string("TestShader_Tex_Calibration"))->second.SetUniform1UI("tex", 0);
	m_shaderMap.find(std::string("TestShader_Tex_Calibration"))->second.SetUniform1F("testVal", 1.0f);

	GLuint textureArray;
	if(m_viewState.m_IsNear)
	{
		textureArray = m_stop.Get();
	}
	else
	{
		textureArray = m_stop.Get() ;
	}

	ShaderProgram shad = m_shaderMap.find(std::string("TestShader_Tex_Calibration"))->second;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureArray);
	Position p (-1,-1,0);
	size 	 s (2,2);
	RenderScreenQuadAtOffset(shad, p,s);

	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void RenderScene::SceneBody_Decision()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(50.0, 0.0, 50.0, 0.0);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);

	glUseProgram(m_shaderMap.find(std::string("TestShader_Tex_Calibration"))->second.programID);

	m_shaderMap.find(std::string("TestShader_Tex_Calibration"))->second.SetUniform1UI("tex", 0);
	m_shaderMap.find(std::string("TestShader_Tex_Calibration"))->second.SetUniform1F("testVal", 1.0f);

	GLuint textureArray = m_decision.Get();

	ShaderProgram shad = m_shaderMap.find(std::string("TestShader_Tex_Calibration"))->second;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureArray);
	Position p (-1,-1,0);
	size 	 s (2,2);
	RenderScreenQuadAtOffset(shad, p,s);

	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void RenderScene::Render_Scene()
{
		static float angle = 0.00f;
		glm::mat4 rotation	= glm::rotate(glm::mat4(1.f), (glm::mediump_float)angle, glm::vec3(0,1,0));  
		m_camera.Rotate(rotation);

		std::string shadername =  "TestShader_persp_MRT_near";

		m_shaderMap.find(shadername)->second.SetUniform1F("blendMode"	, m_blendMode);

		if(m_viewState.m_IsNear)
		{
			m_shaderMap.find(shadername)->second.SetUniform1F("near"		, 1.f);
			m_shaderMap.find(shadername)->second.SetUniform1F("nearClip"	, 0.0001f);
			m_shaderMap.find(shadername)->second.SetUniform1F("farClip"		, 5.4f);
			m_shaderMap.find(shadername)->second.SetUniform1F("blendClip"	, 8.1f);
		}
		else
		{
			m_shaderMap.find(shadername)->second.SetUniform1F("near"		, 0.f);
			m_shaderMap.find(shadername)->second.SetUniform1F("nearClip"	, 5.4f);
			m_shaderMap.find(shadername)->second.SetUniform1F("farClip"		, 9999.f);
			m_shaderMap.find(shadername)->second.SetUniform1F("blendClip"	, 8.1f);
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

		m_shaderMap.find(shadername)->second.SetUniform4FP("lightPosArray", (float*)&lightPos[0], 8);
		m_shaderMap.find(shadername)->second.SetUniform4FP("lightColScaleArray", (float*)&lightColSize[0], 8);

		m_shaderMap.find(shadername)->second.SetUniform3FP("cameraPos", glm::value_ptr(m_camera.GetPos()));
		m_shaderMap.find(shadername)->second.SetUniform3FP("cameraDir", glm::value_ptr( glm::normalize( m_camera.GetDir() ) ) );


		//TODO: Switch to read Enums instead of fixed ints.
		if(m_sceneID == 0) 		//Test
			SceneBody_Test( m_shaderMap.find(shadername)->second);
		else if(m_sceneID == 1) //Distance Test
			SceneBody_Distance( m_shaderMap.find(shadername)->second, 
											glm::vec3( -0.9f, -0.f, m_depthControls.depthA )	, 
											glm::vec3( 0.9f, -0.f, m_depthControls.depthB )	);
		else if(m_sceneID == 2) //Rotation
			SceneBody_Rotation( m_shaderMap.find(shadername)->second, 5.5f, 0.002f);
		else if(m_sceneID == 3) //Translation
			SceneBody_Translation(m_shaderMap.find(shadername)->second, 0.5f, 	10.f, 0.010f);
		else if(m_sceneID == 4) //Calibration
			SceneBody_Calibration();
		else if(m_sceneID == 5) //Decision (left or right)
			SceneBody_Decision();
		else if(m_sceneID == 6) //Static
			SceneBody_Static(m_shaderMap.find(shadername)->second);
		else if(m_sceneID == 7) //Stop
			SceneBody_Stop();

		ClearFrameBuffers();
}

void RenderScene::Render_CopyToViews()
{
	glDisable(GL_CULL_FACE);

	glUseProgram(m_shaderMap.find(std::string("TestShader_Tex"))->second.programID);

	m_shaderMap.find(std::string("TestShader_Tex"))->second.SetUniform1UI("tex", 0);
	m_shaderMap.find(std::string("TestShader_Tex"))->second.SetUniform1F("testVal", 1.0f);
	m_shaderMap.find(std::string("TestShader_Tex"))->second.SetMatrix4FV("XYZtoRGB", glm::value_ptr(m_XYZtoRGB));
	m_shaderMap.find(std::string("TestShader_Tex"))->second.SetUniform3FP("blacklevel", glm::value_ptr(m_blackLevel));
	m_shaderMap.find(std::string("TestShader_Tex"))->second.SetUniform3FP("gamma", glm::value_ptr(m_gamma));

	ShaderProgram shad = m_shaderMap.find(std::string("TestShader_Tex"))->second;

	GLuint textureArray[1] =  {	m_FBOList[0].m_renderTextureIndex };

	for(unsigned int i = 0; i < 1; i++)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureArray[i]);
		Position p = m_layoutControl.GetScreenPos(i);
		size 	 s = m_layoutControl.GetScreenSize(i);
		RenderScreenQuadAtOffset(shad, p,s);
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

			if( m_stereoMode == eStereo_Both  || 	
				(m_stereoMode == eStereo_Left && m_viewState.m_IsLeft) ||
				(m_stereoMode == eStereo_Right && !m_viewState.m_IsLeft)
			  )
			{
				glCullFace(GL_BACK);
				{
					Render_Scene();
					Render_CopyToViews();
				}
			}
			else
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport( 0, 0, m_SizeX, m_SizeY);
				glClearColor(0.0, 0.0, 0.0, 0.0);
				glClearDepth(1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

void RenderScene::RenderScreenQuadAtOffset(ShaderProgram& _shad, Position& _offset, size& _size)
{
	_shad.SetUniform4F("positionOffset", _offset.X(), _offset.Y(), _offset.Z(), 0.f);
	_shad.SetUniform2F("scale", _size.W(), _size.H());
	m_genericUnitQuad.Draw();

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

	float speed = 0.25f;

	//Camera Translation

	if( _key == 'w' )
		m_camera.Translate( speed* 1.f	, 0.f			, 0.f			);
	else if( _key == 's' )
		m_camera.Translate( speed*-1.f	, 0.f			, 0.f			);
	else if( _key == 'a' )
		m_camera.Translate( 0.f			, 0.f			, speed*-1.f	);
	else if( _key == 'd' )
		m_camera.Translate( 0.f			, 0.f			, speed*1.f		);
	else if( _key == 'q' )
		m_camera.Translate( 0.f			, speed*-1.f	, 0.f			);
	else if( _key == 'e' )
		m_camera.Translate( 0.f			, speed*1.f		, 0.f			);

	else if( _key == '[')
		m_camera.ChangeFOV(0.01f);
	else if( _key == ']')
		m_camera.ChangeFOV(-0.01f);

	else if	( _key == 't' )
		m_layoutControl.AdjustScreenPos(Position(0.f, 0.005f, 0.f), 0);
	else if	( _key == 'g' )
		m_layoutControl.AdjustScreenPos(Position(0.f, -0.005f, 0.f), 0);
	else if ( _key == 'f' )
		m_layoutControl.AdjustScreenPos(Position(-0.005f, 0.0f, 0.f), 0);
	else if ( _key == 'h' )
		m_layoutControl.AdjustScreenPos(Position(0.005f, 0.0f, 0.f), 0);

	else if ( _key == 'r' )
		m_layoutControl.AdjustScreenSize(size(0.005f, 0.005f), 0);
	else if ( _key == 'y' )
		m_layoutControl.AdjustScreenSize(size(-0.005f, -0.005f), 0);

	else if ( _key == 'm')
	{
	   IncrementMode();
	}

	else if ( _key == 'z')
	{
		IncrementBlendMode();
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













