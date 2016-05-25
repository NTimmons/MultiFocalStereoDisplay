#include "RenderScene.h"
#include <iostream>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Texture::Init(std::string& _path)
{
	std::cerr << "Load Image: " << _path << "\n";

	int w;
	int h;
	int comp;
	unsigned char* image = stbi_load(_path.c_str(), &w, &h, &comp, 0);

	if(image == nullptr)
		std::cerr << "Failed to load texture\n";

	std::cerr << "\t width: " << w << " height: " << h << " comp: " << comp << "\n";

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if(comp == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB , w, h, 0, GL_RGB , GL_UNSIGNED_BYTE, image);
	else if(comp == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(image);
}

void Texture::Bind(GLenum _TextureUnit)
{
   glActiveTexture(_TextureUnit);
   glBindTexture(GL_TEXTURE_2D, m_texture);
} 
