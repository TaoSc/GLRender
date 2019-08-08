#pragma once

#include <iostream>
#include <memory>
#include <string>

#include <GL/glew.h>


class Texture
{
public:
	Texture();
	explicit Texture(std::string const& file);
	Texture(Texture const& texture_to_copy);
	Texture& operator=(Texture const& texture_to_copy);
	~Texture();

	bool load(GLuint const& texture_wrapping = GL_REPEAT, GLuint const& min_filter = GL_LINEAR_MIPMAP_LINEAR, GLuint const& mag_filter = GL_LINEAR);

	GLuint id() const;
	std::string path() const;
	std::string type() const;
	void setImageFile(std::string const& file);


private:
	GLuint m_id;
	std::string m_file;
	std::string const m_type;
};
