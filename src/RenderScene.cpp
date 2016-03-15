#include "RenderScene.h"


void RenderScene::Initialise()
{
	m_PosX = 0;
	m_PosY = 0;

	m_SizeX = 800;
	m_SizeY = 600;

	InitialiseScreenPositions();
}

FBO RenderScene::CreateFrameBuffer(int _width, int _height, GLenum _format)
{
	GLuint framebufferName = 0;
	glGenFramebuffers(1, &framebufferName);
 	glBindFramebuffer(GL_FRAMEBUFFER, framebufferName);

	// Target render texture
 	GLuint renderedTexture;
	glGenTextures(1, &renderedTexture);

 	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

 	// Give an empty image to OpenGL ( the last "0" )
 	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, _width, _height, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

 	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Depth
 	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);

 	glBindRenderbuffer			(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage		(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);
 	glFramebufferRenderbuffer	(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	m_frameBuffers.push_back		(framebufferName);
	m_renderTextures.push_back		(renderedTexture);
	m_depthRenderTextures.push_back	(depthrenderbuffer);

	FBO fbo(framebufferName,renderedTexture, depthrenderbuffer, _width, _height) ;
	m_FBOList.push_back(fbo);
	return fbo;
}

void RenderScene::ApplySingleFrameBuffer(FBO _fbo)
{
 	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _fbo.m_renderTextureIndex, 0);

 	// Set the list of draw buffers.
 	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
 	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

 	glBindFramebuffer(GL_FRAMEBUFFER, _fbo.m_frameBufferIndex);
	glViewport(0,0,_fbo.m_width,_fbo.m_height);
}

void RenderScene::ApplyFourFrameBuffers(FBO _fbo0, FBO _fbo1,FBO _fbo2,FBO _fbo3)
{
 	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _fbo0.m_renderTextureIndex, 0);

 	// Set the list of draw buffers.
 	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
 	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

 	glBindFramebuffer(GL_FRAMEBUFFER, _fbo0.m_frameBufferIndex);
	glViewport(0,0,_fbo0.m_width,_fbo0.m_height);
}

void RenderScene::ClearFrameBuffers()
{
	// Render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShaderProgram* RenderScene::CreateShaderProgramObject(char* _vertexFilename, char* _pixelFilename)
{
	GLuint vertexShader 	= -1;
	GLuint fragmentShader 	= -1;

	GLuint vertexShaderObject 		= glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderObject 	= glCreateShader(GL_FRAGMENT_SHADER);


	char* vShaderSource = 0;
	int vLen 	= 0;

	char* pShaderSource = 0;
	int pLen 	= 0;

	LoadShader(_vertexFilename,&vShaderSource, &vLen);
	LoadShader(_pixelFilename,&pShaderSource, &pLen);

	glShaderSource(vertexShaderObject	, 1, const_cast<const char**>(&vShaderSource), static_cast<GLint*>(&vLen) );
	glShaderSource(fragmentShaderObject	, 1, const_cast<const char**>(&pShaderSource), static_cast<GLint*>(&pLen) );


	//C ompile
	glCompileShader(vertexShaderObject);	

	if(!validateCompilation(vertexShaderObject))
	{ return NULL;}

	glCompileShader(fragmentShaderObject);
	if(!validateCompilation(fragmentShaderObject))
	{ return  NULL;}


	// Link
	GLuint ProgramObject = glCreateProgram();

	glAttachShader(ProgramObject, vertexShaderObject);
	glAttachShader(ProgramObject, fragmentShaderObject);

	glLinkProgram(ProgramObject); 
	if(!validateLinking(ProgramObject, vertexShaderObject,  fragmentShaderObject))
	{ return  NULL;}

	ShaderProgram shaderObject = ShaderProgram( ProgramObject);
	m_shaderList.push_back(shaderObject);

	return &m_shaderList[m_shaderList.size()-1];
}

bool RenderScene::validateCompilation(GLuint _shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(_shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(_shader, maxLength, &maxLength, &infoLog[0]);
	
		//We don't need the shader anymore.
		glDeleteShader(_shader);

		//Use the infoLog as you see fit.
	
		//In this simple program, we'll just leave
		return false;
	}

	return true;

}

bool RenderScene::validateLinking(GLuint program, GLuint vertexShader, GLuint fragmentShader)
{
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if(isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
	
		//We don't need the program anymore.
		glDeleteProgram(program);
		//Don't leak shaders either.
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		//Use the infoLog as you see fit.
	
		//In this simple program, we'll just leave
		return false;
	}

	return true;
}


int RenderScene::GetFileLength(std::ifstream& file)
{
    if(!file.good()) return 0;
    
    unsigned int pos=file.tellg();
    file.seekg(0,std::ios::end);
    unsigned int len = file.tellg();
    file.seekg(std::ios::beg);
    
    return len;
}




int RenderScene::LoadShader(char* filename, char** ShaderSource, int* len)
{
	std::ifstream file;
	file.open(filename, std::ios::in); // opens as ASCII!
	if(!file) 
		return -1;

	*len = GetFileLength(file);

	if (len==0) 
		return -2;   // Error: Empty File 
    
	*ShaderSource =  new char[*len+1];
	if (*ShaderSource == 0) 
		return -3;   // can't reserve memory

	// len isn't always strlen cause some characters are stripped in ascii read...
	// it is important to 0-terminate the real length later, len is just max possible value... 
	*ShaderSource[*len] = 0; 

	int i=0;
	while (file.good())
	{
		*ShaderSource[i] = file.get();       // get character from file.
		if (!file.eof())
			i++;
	}

	*ShaderSource[i] = 0;  // 0-terminate it at the correct position

	file.close();
	  
	return 0; // No Error
}


int RenderScene::UnloadShader(GLubyte** ShaderSource)
{
	if (*ShaderSource != 0)
		delete[] *ShaderSource;
	*ShaderSource = 0;
}

void RenderScene::Render()
{
	glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluOrtho2D(0, m_SizeX, 0, m_SizeY); 
    glViewport(0, 0, m_SizeX, m_SizeY);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glCullFace(GL_NONE);	
    {
        glBegin(GL_TRIANGLES);

		RenderQuad(m_screenArray[0].m_pos, m_screenArray[0].m_size, m_screenArray[0].m_colour);
		RenderQuad(m_screenArray[1].m_pos, m_screenArray[1].m_size, m_screenArray[1].m_colour);
		RenderQuad(m_screenArray[2].m_pos, m_screenArray[2].m_size, m_screenArray[2].m_colour);  
		RenderQuad(m_screenArray[3].m_pos, m_screenArray[3].m_size, m_screenArray[3].m_colour);  

        glEnd();
    }

    glFlush();
    glutSwapBuffers() ;

}

void RenderScene::InitialiseScreenPositions()
{
	m_screenArray[0].m_pos = position(0.f     , 100.f, 0.f);
	m_screenArray[1].m_pos = position(200.f   , 100.f, 0.f);
	m_screenArray[2].m_pos = position(400.f   , 100.f, 0.f);
	m_screenArray[3].m_pos = position(600.f   , 100.f, 0.f);

	m_screenArray[0].m_size = size(100.f, 100.0f);
	m_screenArray[1].m_size = size(100.f, 100.0f);
	m_screenArray[2].m_size = size(100.f, 100.0f);
	m_screenArray[3].m_size = size(100.f, 100.0f);

	m_screenArray[0].m_colour = colour(1.f, 0.f, 0.f, 1.f);
	m_screenArray[1].m_colour = colour(0.f, 1.f, 0.f, 1.f);
	m_screenArray[2].m_colour = colour(0.f, 0.f, 1.f, 1.f);
	m_screenArray[3].m_colour = colour(1.f, 1.f, 0.f, 1.f);
}












