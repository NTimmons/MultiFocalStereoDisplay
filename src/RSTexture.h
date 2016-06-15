#ifndef _INCL_TEXTURE
#define _INCL_TEXTURE

class Texture
{
public:
	Texture(): m_texture(-1){}

	void Init(std::string& _path);
	
	GLuint Get(){ return m_texture; }

	void Bind(GLenum _TextureUnit);

	GLuint m_texture;
};

#endif