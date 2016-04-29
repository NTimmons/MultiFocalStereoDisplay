#include "RenderScene.h"
#include <iostream>


GLint ShaderProgram::FetchUniformValue(std::string _name)
{
	auto res = m_uniformMap.find(_name);
	if(res == m_uniformMap.end())
	{
		std::cerr << "Uniform not in map. Attempting to fetch: " << _name.c_str() << "\n";

		 		
		glUseProgram(programID);	 
		GLint loc = glGetUniformLocation(programID, _name.c_str());
		glUseProgram(0);

		if(loc == -1)
		{
			std::cerr << "\tError: " << _name << " is not a uniform in " << name << " \n";
			return -1;
		}		
		else
		{
			std::cerr << "\tFound uniform: " << _name << " at location " << loc << ". Storing...\n";
			m_uniformMap.insert( std::make_pair(_name, loc) );

			return loc;
		}
	}
	else
	{
		return res->second;
	}

	return -1;
}	


void  ShaderProgram::SetUniform1UI(std::string _name, GLuint _val)
{
	GLint uniLoc = FetchUniformValue(_name);	
	if(uniLoc == -1)
	{
		return;
	}

	glUseProgram(programID);	
	glUniform1i(uniLoc, (GLint)_val);
}
void  ShaderProgram::SetUniform1F(std::string _name, float _val)
{
	GLint uniLoc = FetchUniformValue(_name);	
	if(uniLoc == -1)
	{
		return;
	}
	glUseProgram(programID);
	glUniform1f(uniLoc, _val);
}

void  ShaderProgram::SetUniform2F(std::string _name, float _val0, float _val1)
{
	GLint uniLoc = FetchUniformValue(_name);	
	if(uniLoc == -1)
	{
		return;
	}

	glUseProgram(programID);
	glUniform2f(uniLoc,  _val0,_val1);
}

void  ShaderProgram::SetUniform3F(std::string _name, float _val0, float _val1, float _val2)
{
	GLint uniLoc = FetchUniformValue(_name);	
	if(uniLoc == -1)
	{
		return;
	}

	glUseProgram(programID);
	glUniform3f(uniLoc,  _val0,_val1,_val2);
}

void  ShaderProgram::SetUniform4F(std::string _name, float _val0, float _val1, float _val2, float _val3)
{
	GLint uniLoc = FetchUniformValue(_name);	
	if(uniLoc == -1)
	{
		return;
	}

	glUseProgram(programID);	 
	glUniform4f(uniLoc, _val0,_val1,_val2,_val3);	 
}


void  ShaderProgram::SetUniform2FP(std::string _name, float* _val)
{
	GLint uniLoc = FetchUniformValue(_name);	
	if(uniLoc == -1)
	{
		return;
	}

	glUseProgram(programID);
	glUniform2fv(uniLoc, 1,_val);
		 
}
void  ShaderProgram::SetUniform3FP(std::string _name, float* _val)
{
	GLint uniLoc = FetchUniformValue(_name);	
	if(uniLoc == -1)
	{
		return;
	}

	glUseProgram(programID);
	glUniform3fv(uniLoc, 1,_val);
		 
}
void  ShaderProgram::SetUniform4FP(std::string _name, float* _val)
{
	GLint uniLoc = FetchUniformValue(_name);	
	if(uniLoc == -1)
	{
		return;
	}
		 
	glUseProgram(programID);
	glUniform4fv(uniLoc, 1,_val);
}

void ShaderProgram::SetMatrix4FV(std::string _name, const GLfloat* _val)
{
	GLint uniLoc = FetchUniformValue(_name);	
	if(uniLoc == -1)
	{
		return;
	}
		 
	glUseProgram(programID);
	glUniformMatrix4fv(uniLoc, 1,GL_FALSE, _val);

}


bool RenderScene::CreateShaderProgramObject(std::string& _vertexFilename, std::string& _pixelFilename, std::string _name)
{
	std::cerr << "Creating shader '" << _name << "' from:" << _vertexFilename << "," << _pixelFilename << "\n";
	
	GLuint vertexShaderObject 		= glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderObject 	= glCreateShader(GL_FRAGMENT_SHADER);

	//std::cerr << _name << ": Created GL Shader Objects\n";

	std::string vShaderSource = "";
	GLint vLen 	= 0;

	std::string pShaderSource = "";
	GLint pLen 	= 0;

	//std::cerr << _name << ": Preparing to load vertex shader source code...\n";
	LoadShader(_vertexFilename, vShaderSource, &vLen);


	//std::cerr << _name << ": Preparing to load fragment shader source code...\n";
	LoadShader(_pixelFilename, pShaderSource, &pLen);
	
	const char* vSource = vShaderSource.c_str();
	const char* pSource = pShaderSource.c_str();
	//std::cerr << vSource << "\n";
	//std::cerr << pSource << "\n";


	vLen = vShaderSource.length();
	pLen = pShaderSource.length();

	//std::cerr << _name << ": Applying shader source to shader object (vertex)...\n";
	glShaderSource(vertexShaderObject	, 1, &vSource, (&vLen) );
	//std::cerr << _name << ": Applying shader source to shader object (fragment)...\n";
	glShaderSource(fragmentShaderObject	, 1, &pSource, (&pLen) );


	// Compile
	//std::cerr << _name << ": Compiling shader object (vertex)...\n";
	glCompileShader(vertexShaderObject);	

	if(!validateCompilation(vertexShaderObject))
	{ return false;}

	//std::cerr << _name << ": Compiling shader object (fragment)...\n";
	glCompileShader(fragmentShaderObject);
	if(!validateCompilation(fragmentShaderObject))
	{ return  false;}


	// Link
	//std::cerr << _name << ": Creating shader program...\n";
	GLuint ProgramObject = glCreateProgram();

	//std::cerr << _name << ": Attaching vertex shader to shader program...\n";
	glAttachShader(ProgramObject, vertexShaderObject);
	//std::cerr << _name << ": Attaching fragment shader to shader program...\n";
	glAttachShader(ProgramObject, fragmentShaderObject);

	//std::cerr << _name << ": Linking shader program...\n";
	glLinkProgram(ProgramObject); 
	//std::cerr << _name << ": Validating linked program: " << ProgramObject << ".\n";
	bool linkSuccess = validateLinking(ProgramObject, vertexShaderObject,  fragmentShaderObject);

	if(!linkSuccess)
	{ 
		std::cerr << _name << ": Complete:FAILED";
		return  false;
	}

	std::cerr << _name << ": Complete:";

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

		//std::cerr << "\tFetching infolog length...";
		GLint maxLength = 0;
		glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &maxLength);
		std::cerr << maxLength << "\n";

		std::cerr << "\tFetching infolog...\n";	
		GLchar* infoLog = new GLchar[maxLength];

		glGetShaderInfoLog(_shader, maxLength, &maxLength, &infoLog[0]);
		std::cerr << "Shader Error:>"<< infoLog << "\n";
	
		//We don't need the shader anymore.
		//std::cerr << "\tRemoving Shader: " << _shader << "\n";	
		glDeleteShader(_shader);

		//Use the infoLog as you see fit.
	
		//In this simple program, we'll just leave
		std::cerr << "\tCompiling Failed\n";

		delete infoLog;

		return false;
	}

	//std::cerr << "\tCompiled: " << _shader << " successfully\n";

	return true;

}

bool RenderScene::validateLinking(GLuint program, GLuint vertexShader, GLuint fragmentShader)
{
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if(isLinked == GL_FALSE)
	{
		std::cerr << "\tLinking Failed. Getting error information.\n";

		//std::cerr << "\tFetching infolog length...";
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		std::cerr << maxLength << "\n";

		//The maxLength includes the NULL character
		std::cerr << "\tFetching infolog...\n";
		GLchar* infoLog = new GLchar[maxLength];
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		std::cerr << "Shader Errors:>"<< infoLog << "\n";


		//std::cerr << "\tRemoving program and compiled shaders.\n";
		//We don't need the program anymore.
		//std::cerr << "\tRemoving Program: " << program << "\n";
		glDeleteProgram(program);

		//std::cerr << "\tRemoving Shader: " << vertexShader << "\n";
		//Don't leak shaders either.
		glDeleteShader(vertexShader);

		//std::cerr << "\tRemoving Shader: " << fragmentShader << "\n";
		glDeleteShader(fragmentShader);

		//Use the infoLog as you see fit.
	
		//In this simple program, we'll just leave

		std::cerr << "\tLinking Failed\n";

		delete infoLog;

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

	//std::cerr << "\t Preparing to open: " << filename << ".";

	std::ifstream file;
	file.open(filename.c_str(), std::ios::in); // opens as ASCII!
	if(!file)
	{ 
		//std::cerr << "\t ...FAILED\n";
		return -1;
	}
	
	//std::cerr << "\t ...Success\n";


	//std::cerr << "\t Getting file length...";
	*len = GetFileLength(file);

	if (len==0) 
	{
		//std::cerr << "\t FAILED (Empty File)\n";
		return -2;   // Error: Empty File 
	}


	//std::cerr << "\t Success (size:" << *len << ")\n";

	std::string line;
	if (file.is_open())
	{
		//std::cerr << "\t Reading>";
		while ( getline (file,line) )
		{
		  	ShaderSource.append("\n");
		  	ShaderSource.append(line);
			//std::cerr << "-";	
		}
		//std::cerr << "\n";
		//std::cerr << "\t Closing file.\n";
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

