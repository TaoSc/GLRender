#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


Texture::Texture() : m_id(0), m_file()
{
}

Texture::Texture(std::string const& file) : m_id(0), m_file(file)
{
}

Texture::Texture(Texture const& texture_to_copy) : m_id(0), m_file(texture_to_copy.m_file)
{
	load();
}

Texture& Texture::operator=(Texture const& texture_to_copy)
{
	m_file = texture_to_copy.m_file;
	load();

	return *this;
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_id);
}


bool Texture::load(GLuint const& texture_wrapping, GLuint const& min_filter, GLuint const& mag_filter)
{
	stbi_set_flip_vertically_on_load(true);

	if (glIsTexture(m_id) == GL_TRUE)
		glDeleteTextures(1, &m_id);

	glGenTextures(1, &m_id);

	int width, height, nb_channels;
	unsigned char* const data = stbi_load(m_file.c_str(), &width, &height, &nb_channels, 0);

	if (!data)
	{
		std::cout << "Texture failed to load, path: " << m_file << std::endl;
		stbi_image_free(data);

		return false;
	}

	GLenum format(0);
	if (nb_channels == 1)
		format = GL_RED;
	else if (nb_channels == 3)
		format = GL_RGB;
	else if (nb_channels == 4)
		format = GL_RGBA;

	glBindTexture(GL_TEXTURE_2D, m_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture_wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture_wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

	float aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);

	return true;
}


GLuint Texture::id() const
{
	return m_id;
}

std::string Texture::path() const
{
	return m_file;
}

std::string Texture::type() const
{
	return m_type;
}

void Texture::setImageFile(std::string const& file)
{
	m_file = file;
}
