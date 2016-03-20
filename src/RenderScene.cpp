#include "RenderScene.h"


#include <iostream>


void TestGLError(const char* _file, int _line)
{
	GLenum id = glGetError();

	if(GL_NO_ERROR == id)
		return;
	else
		std::cerr << "OpenGL Error: (" << _file << ", " << _line << ")-> " << id << "\n";
}

void QuadMesh::Draw()
{
	static bool firstDraw = true;
	if(firstDraw)
	{
		std::cerr << "Drawing QUAD\n";
		firstDraw = false;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	// Draw the triangles !
	/*glDrawElements	( GL_TRIANGLES,      // mode
					  6,    // count
					  GL_UNSIGNED_INT,   // type
					  (void*)0           // element array buffer offset
					);
*/
	glDrawArrays( GL_TRIANGLE_FAN, 0, 4);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void QuadMesh::Initialise()
{
	std::cerr << "Initialising QuadMesh\n";

	std::cerr << "\tInitialising vertex data...\n";

	vertices[0].m_pos = Position(0.f, 0.f, 0.f);
	vertices[1].m_pos = Position(0.f, 1.f, 0.f);
	vertices[2].m_pos = Position(1.f, 1.f, 0.f);
	vertices[3].m_pos = Position(1.f, 0.f, 0.f);

	
	vertices[0].m_col = colour(0.f, 0.f, 0.f, 1.f);
	vertices[1].m_col = colour(0.f, 1.f, 0.f, 1.f);
	vertices[2].m_col = colour(1.f, 1.f, 0.f, 1.f);
	vertices[3].m_col = colour(1.f, 0.f, 0.f, 1.f);

	vertices[0].m_size = size(0.f, 0.f);
	vertices[1].m_size = size(0.f, 1.f);
	vertices[2].m_size = size(1.f, 0.f);
	vertices[3].m_size = size(1.f, 1.f);

	vertices[0].m_uv = texCoord(0.f, 0.f);
	vertices[1].m_uv = texCoord(0.f, 1.f);
	vertices[2].m_uv = texCoord(1.f, 0.f);
	vertices[3].m_uv = texCoord(1.f, 1.f);
	

/*
	std::cerr << "\tInitialising index data...\n";
	indicies[0] = 0;
	indicies[1] = 1;
	indicies[2] = 3;
	indicies[3] = 0;
	indicies[4] = 3;
	indicies[5] = 1;

	std::cerr << "\tCreating index buffer object...\n";
	//Index Buffer Objects
	glGenBuffers(1, &elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &indicies[0], GL_STATIC_DRAW);
	std::cerr << "\tCreated index buffer object at id: " << elementBufferObject << "\n";*/

	//Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertex)*4, &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);  // Vertex position.
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)0);

	glEnableVertexAttribArray(1);  // Vertex color.
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)16);

	glEnableVertexAttribArray(2);  // Vertex size.
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)32);

	glEnableVertexAttribArray(3);  // Vertex uv.
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)40);

	std::cerr << "\tCreated vertex buffer object at id: " << vertexBufferObject << "\n";

	//Cleanup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}


void RenderScene::InitialiseRenderObjects()
{
	m_genericUnitQuad.Initialise();
}

void RenderScene::Initialise()
{
	m_PosX = 0;
	m_PosY = 0;

	m_SizeX = 800;
	m_SizeY = 600;

	glViewport(0,0,1,1);

	InitialiseScreenPositions();
}

FBO RenderScene::CreateFrameBuffer(int _width, int _height, GLenum _format)
{
	(void)_format;


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

 	glBindRenderbuffer				(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage			(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);
 	glFramebufferRenderbuffer		(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

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

	//TODO OTHER FRAME BUFFERS.
	(void)_fbo1;
	(void)_fbo2;
	(void)_fbo3;

}

void RenderScene::ClearFrameBuffers()
{
	// Render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool RenderScene::CreateShaderProgramObject(std::string& _vertexFilename, std::string& _pixelFilename, std::string _name)
{
	std::cerr << "Creating shader from:" << _vertexFilename << "," << _pixelFilename << "\n";
	
	GLuint vertexShaderObject 		= glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderObject 	= glCreateShader(GL_FRAGMENT_SHADER);

	std::cerr << "Created GL Shader Objects\n";

	std::string vShaderSource = "";
	GLint vLen 	= 0;

	std::string pShaderSource = "";
	GLint pLen 	= 0;

	std::cerr << "Preparing to load vertex shader source code...\n";
	LoadShader(_vertexFilename, vShaderSource, &vLen);


	std::cerr << "Preparing to load fragment shader source code...\n";
	LoadShader(_pixelFilename, pShaderSource, &pLen);
	


	const char* vSource = vShaderSource.c_str();
	const char* pSource = pShaderSource.c_str();
	//std::cerr << vSource << "\n";
	//std::cerr << pSource << "\n";


	vLen = vShaderSource.length();
	pLen = pShaderSource.length();

	std::cerr << "Applying shader source to shader object (vertex)...\n";
	glShaderSource(vertexShaderObject	, 1, &vSource, (&vLen) );
	std::cerr << "Applying shader source to shader object (fragment)...\n";
	glShaderSource(fragmentShaderObject	, 1, &pSource, (&pLen) );


	// Compile
	std::cerr << "Compiling shader object (vertex)...\n";
	glCompileShader(vertexShaderObject);	

	if(!validateCompilation(vertexShaderObject))
	{ return false;}

	std::cerr << "Compiling shader object (fragment)...\n";
	glCompileShader(fragmentShaderObject);
	if(!validateCompilation(fragmentShaderObject))
	{ return  false;}


	// Link
	std::cerr << "Creating shader program...\n";
	GLuint ProgramObject = glCreateProgram();

	std::cerr << "Attaching vertex shader to shader program...\n";
	glAttachShader(ProgramObject, vertexShaderObject);
	std::cerr << "Attaching fragment shader to shader program...\n";
	glAttachShader(ProgramObject, fragmentShaderObject);


	std::cerr << "Linking shader program...\n";
	glLinkProgram(ProgramObject); 
	std::cerr << "Validating linked program: " << ProgramObject << ".\n";
	bool linkSuccess = validateLinking(ProgramObject, vertexShaderObject,  fragmentShaderObject);

	if(!linkSuccess)
	{ 
		std::cerr << "Complete:FAILED";
		return  false;
	}

	std::cerr << "Complete:";

	//ShaderProgram shaderObject = ;

	m_shaderMap.insert(std::make_pair(_name,  ShaderProgram( ProgramObject, _name) ) );

	std::cerr << "SUCCESS.\n";

	return true;
}

bool RenderScene::validateCompilation(GLuint _shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(_shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		std::cerr << "\tLinking Failed. Getting error information.\n";

		std::cerr << "\tFetching infolog length...";
		GLint maxLength = 0;
		glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &maxLength);
		std::cerr << maxLength << "\n";

		std::cerr << "\tFetching infolog...\n";	
		GLchar infoLog[maxLength];
		glGetShaderInfoLog(_shader, maxLength, &maxLength, &infoLog[0]);
		std::cerr << ""<< infoLog << "\n";
	
		//We don't need the shader anymore.
		std::cerr << "\tRemoving Shader: " << _shader << "\n";	
		glDeleteShader(_shader);

		//Use the infoLog as you see fit.
	
		//In this simple program, we'll just leave
		std::cerr << "\tCompiling Failed\n";
		return false;
	}

	std::cerr << "\tCompiled: " << _shader << " successfully\n";

	return true;

}

bool RenderScene::validateLinking(GLuint program, GLuint vertexShader, GLuint fragmentShader)
{
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if(isLinked == GL_FALSE)
	{
		std::cerr << "\tLinking Failed. Getting error information.\n";

		std::cerr << "\tFetching infolog length...";
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		std::cerr << maxLength << "\n";

		//The maxLength includes the NULL character
		std::cerr << "\tFetching infolog...\n";
		GLchar infoLog[maxLength];
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		std::cerr << ""<< infoLog << "\n";


		std::cerr << "\tRemoving program and compiled shaders.\n";
		//We don't need the program anymore.
		std::cerr << "\tRemoving Program: " << program << "\n";
		glDeleteProgram(program);

		std::cerr << "\tRemoving Shader: " << vertexShader << "\n";
		//Don't leak shaders either.
		glDeleteShader(vertexShader);

		std::cerr << "\tRemoving Shader: " << fragmentShader << "\n";
		glDeleteShader(fragmentShader);

		//Use the infoLog as you see fit.
	
		//In this simple program, we'll just leave

		std::cerr << "\tLinking Failed\n";
		return false;
	}

	return true;
}

int RenderScene::GetFileLength(std::ifstream& file)
{
    if(!file.good()) return 0;
    
    //unsigned int pos=file.tellg();
    file.seekg(0,std::ios::end);
    unsigned int len = file.tellg();
    file.seekg(std::ios::beg);
    
    return len;
}

int RenderScene::LoadShader(std::string& filename, std::string& ShaderSource, int* len)
{

	std::cerr << "\t Preparing to open: " << filename << ".";

	std::ifstream file;
	file.open(filename.c_str(), std::ios::in); // opens as ASCII!
	if(!file)
	{ 
		std::cerr << "\t ...FAILED\n";
		return -1;
	}
	
	std::cerr << "\t ...Success\n";


	std::cerr << "\t Getting file length...";
	*len = 0;//GetFileLength(file);

	if (len==0) 
	{
		std::cerr << "\t FAILED (Empty File)\n";
		return -2;   // Error: Empty File 
	}


	std::cerr << "\t Success (size:" << *len << ")\n";

	std::string line;
	if (file.is_open())
	{

		while ( getline (file,line) )
		{
		  	ShaderSource.append("\n");
		  	ShaderSource.append(line);
			std::cerr << " ... ";	
		}
		std::cerr << "\n";
		std::cerr << "\t Closing file.\n";
		file.close();

	}

	//std::cerr << ShaderSource << "\n";
	return 0; // No Error
}


int RenderScene::UnloadShader(GLubyte** ShaderSource)
{
	if (*ShaderSource != 0)
		delete[] *ShaderSource;
	*ShaderSource = 0;

	return 0;
}

void RenderScene::Render()
{

    glClearColor(0.3, 0.4, 0.8, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

/*
	glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluOrtho2D(0, m_SizeX, 0, m_SizeY); 
    glViewport(0, 0, m_SizeX, m_SizeY);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);



*/
    glCullFace(GL_NONE);	

    {
/*
        glBegin(GL_TRIANGLES);

		RenderQuad(m_screenArray[0].m_pos, m_screenArray[0].m_size, m_screenArray[0].m_colour);
		RenderQuad(m_screenArray[1].m_pos, m_screenArray[1].m_size, m_screenArray[1].m_colour);
		RenderQuad(m_screenArray[2].m_pos, m_screenArray[2].m_size, m_screenArray[2].m_colour);  
		RenderQuad(m_screenArray[3].m_pos, m_screenArray[3].m_size, m_screenArray[3].m_colour);  

        glEnd();
*/

		glUseProgram(m_shaderMap.find(std::string("TestShader"))->second.programID);
		//size newSize = size(500.f, 500.0f);

		//Set texture
		/*
		{

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[0]);
		}
		*/


        //glBegin(GL_TRIANGLES);
			m_genericUnitQuad.Draw();
		//glEnd();

		glUseProgram(0);


    }

    glFlush();
    glutSwapBuffers() ;

}

void RenderScene::InitialiseScreenPositions()
{
	
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












