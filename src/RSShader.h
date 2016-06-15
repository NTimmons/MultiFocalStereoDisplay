#ifndef _INCL_SHADER
#define _INCL_SHADER

class ShaderProgram
{
	public:
	ShaderProgram(GLuint _id): programID(_id), name("Unnamed"){}
	ShaderProgram(GLuint _id, std::string& _name): programID(_id), name(_name){}

	GLint FetchUniformValue(std::string _name);

	void SetUniform1UI(std::string _name, GLuint _val);
	void SetUniform1F(std::string _name, float _val);
	void SetUniform2F(std::string _name, float _val0, float _val1);
	void SetUniform3F(std::string _name, float _val0, float _val1, float _val2);
	void SetUniform4F(std::string _name, float _val0, float _val1, float _val2, float _val3);

	void SetUniform2FP(std::string _name, float* _val);
	void SetUniform3FP(std::string _name, const float* _val);
	void SetUniform4FP(std::string _name, const float* _val);
	void SetUniform4FP(std::string _name, const float* _val, int _count);

	void SetMatrix4FV(std::string _name, const GLfloat* _val);

	std::map<std::string, GLint> m_uniformMap;	
	GLuint programID;
	std::string name;
};


#endif