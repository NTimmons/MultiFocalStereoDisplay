#ifndef _INCL_RENDER_SCENE
#define _INCL_RENDER_SCENE

#include <vector>

#include <GL/glew.h>
//#include "GL/freeglut.h"

#include <fstream>
#include <iostream>
#include <map>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "TestController.h"

#include "RSCamera.h"
#include "RSMesh.h"
#include "RSShader.h"
#include "RSScreenLayout.h"
#include "RSFrameBufferObjects.h"
#include "RSTexture.h"
#include "VectorTypes.h"

#define TESTGL 	RenderScene::TestGLError(__FILE__, __LINE__)

//TODO NEEDS REFACTORING

enum ERENDERMODE
{
	E_MODELSCENE,
	E_RED,
	E_GREEN,
	E_BLUE,
	E_BLANK,
	E_END
};

class Material
{
public:
	Material( 	std::string _name, 
				colour _ambient, colour _diffuse, colour _spec, colour _emissive,
				float _spec_att, float _spec_pow, Texture _specular_map, Texture _diffuse_map, Texture _bump_map)
	: 	ambient(_ambient),
	  	diffuse(_diffuse),
		specular(_spec),
		emissive(_emissive),
		spec_att(_spec_att),
		spec_pow(_spec_pow),
		specular_map(_specular_map),
		diffuse_map(_diffuse_map),
		bump_map(_bump_map)
	{
		std::cerr << "Material Initialised: " << _name.c_str() << "\n";
	}

	Material()
	:	ambient (0.f, 0.f, 0.f, 0.f),
	  	diffuse (0.f, 0.f, 0.f, 0.f),
		specular(0.f, 0.f, 0.f, 0.f),
		emissive(0.f, 0.f, 0.f, 0.f),
		spec_att(0.f),
		spec_pow(0.f)
	{
		std::cerr << "Material Initialised: NO PARAMS" << "\n";
	}

	void 	Apply(ShaderProgram _shad);
			
	colour 	ambient;
	colour 	diffuse;
	colour 	specular;
	colour 	emissive;

	float 	spec_att;
	float 	spec_pow;

	Texture specular_map;
	Texture diffuse_map;
	Texture bump_map;
};


struct ViewState
{
	ViewState(bool _left, bool _near) : m_IsNear(_near), m_IsLeft(_left){}
	ViewState() : m_IsNear(true), m_IsLeft(true){}
	bool m_IsNear;
	bool m_IsLeft;
};

struct PointLight
{
	PointLight()
	{
		m_pos 		= Position(0.f, 0.f, 0.f);
		m_colour 	= colour(1.f, 1.f, 0.f, 1.f);
		m_distance 	= 20.f;
	}

	void Set(glm::vec3 _pos, glm::vec3 _col, float _dist)
	{
		m_pos 	 = Position(_pos.x, _pos.y, _pos.z);
		m_colour = colour(_col.x, _col.y, _col.z, 1.0f);
		m_distance = _dist;
	}

	Position m_pos;
	colour 	 m_colour;
	float	 m_distance;
};

class RenderScene
{
public:
	RenderScene()
	{ 
		m_renderMode = E_MODELSCENE;
		m_blendMode = 3.f;
		m_sceneID = 0;
		m_stereoMode = eStereo_Both;
	}

	//Setup
	void 			Initialise();
	void 			InitialiseRenderObjects();
	static void 	TestGLError(const char* _file, int _line);
	void 			IncrementMode();
	void 			IncrementBlendMode();
	void 			SetBlendMode(float _blendMode);
	void 			SetScene( int _sceneID);
	void 			SetCamera(Camera& _cam);
	void 			SetLeftRight(bool _left);
	bool 			LoadMaterial( std::string _file,  std::string _name);

	AIMesh* GetMesh( std::string _name)
	{
		for(unsigned int i = 0; i < m_meshArray.size(); i++)
		{
			if(m_meshArray[i].m_name.compare(_name) == 0)
			{
				return &m_meshArray[i];
			}
		}

		return 0;
	}
	
	//Rendering
	void Render();
	void Render_CopyToViews();
	void Render_Scene();

	//Scenes -:
	void SceneBody_Test			(ShaderProgram& _prog);
	void SceneBody_Distance		(ShaderProgram& _prog, glm::vec3 _left	, glm::vec3 _right);
	void SceneBody_Rotation		(ShaderProgram& _prog, float _depth		, float _speed		);
	void SceneBody_Translation	(ShaderProgram& _prog, float _depthMin	, float _depthMax	, 	float _speed);
	void SceneBody_Calibration();
	void SceneBody_Stop();
	void SceneBody_Decision();
	void SceneBody_Static(ShaderProgram& _prog);

	void RenderQuad(Position& _pos, size& _size, colour& _col);
	void RenderScreenQuadAtOffset(ShaderProgram& _shad, Position& _offset, size& _size);

	void SetLight( glm::vec3 _pos, glm::vec3 _colour, float _scale, int _index);

	void SetStereoMode( StereoModes _mode)
	{
		m_stereoMode = _mode;
	}

	void SetDepthComparison( float _da, float _db, float _dc, float _sa, float _sb, float _sc, float _ra, float _rb)
	{
		 m_depthControls = DepthObjects( _da, _db, _dc, _sa, _sb, _sc, _ra, _rb);
	}

	//Screen Control
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

	void SetNearFar(bool _near)
	{
		m_viewState.m_IsNear = _near;
	}

	void SetID( int _id)
	{
		m_id = _id;
	}
	
	void SetXYZtoRGBMat	( glm::mat4& _mat	);
	void SetBlackLevel	( glm::vec3& _blk	)
	{
		m_blackLevel = _blk;
	}
	void SetGamma		( glm::vec3& _gamma	)
	{
		m_gamma = _gamma;
	}

	glm::mat4 m_XYZtoRGB;
	glm::vec3 m_blackLevel;
	glm::vec3 m_gamma;

	int m_id;

	// Lists and lists and lists
	std::vector<GLuint> 		m_frameBuffers;
	std::vector<GLuint> 		m_renderTextures;
	std::vector<GLuint> 		m_depthRenderTextures;
	std::vector<FBO> 			m_FBOList;
	std::vector<FBOMulti> 		m_FBOMultiList;
	
	std::map<std::string, ShaderProgram> m_shaderMap;

	std::map<std::string, Material> m_materialMap;

	ScreenLayout				m_layoutControl;

	QuadMesh					m_genericUnitQuad;
	AIMesh						m_boxMesh;

	Texture						m_testTexture;
	Texture						m_nearCalibration;
	Texture						m_farCalibration;
	Texture						m_stop;
	Texture						m_decision;

	//Render Settings
	Camera 						m_camera;
	ViewState					m_viewState;
	ERENDERMODE					m_renderMode;
	PointLight					m_pointLights[8];
	float						m_blendMode;
	std::vector<AIMesh>			m_meshArray;
	int 						m_sceneID;

	struct DepthObjects
	{
		DepthObjects(){};
		DepthObjects( float _da, float _db, float _dc, float _sa, float _sb, float _sc, float _ra, float _rb)
			: depthA(_da), depthB(_db), depthC(_dc), scaleA(_sa), scaleB(_sb), scaleC(_sc), rotA(_ra), rotB(_rb) {}

		float depthA;
		float depthB;
		float depthC;

		float scaleA;
		float scaleB;
		float scaleC;

		float rotA;
		float rotB;
	};

	DepthObjects m_depthControls;
	StereoModes	m_stereoMode;

	//Window Controls
	int m_PosX;
	int m_PosY;

	int m_SizeX;
	int m_SizeY;
};


void ScreenWriteToFile	(std::string _path, RenderScene* _RS);

#endif







