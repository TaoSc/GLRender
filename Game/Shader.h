#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <GL/glew.h>


class Shader
{
public:
	Shader(std::string const& vertex_shader_source_file, std::string const& fragment_shader_source_file);
	~Shader();

	GLuint id() const;

	void setUni(std::string const& name, bool const& value) const;
	void setUni(std::string const& name, int const& value) const;
	void setUni(std::string const& name, float const& value) const;
	void setUni(std::string const& name, glm::mat4 const& value) const;
	void setUni(std::string const& name, float const& value1, float const& value2, float const& value3) const;
	void setUni(std::string const& name, glm::vec3 const& value) const;


private:
	static GLuint compile(std::string const& file_path, GLuint const& type);
	void link();

	GLuint m_shader_program_id;
	std::string const m_vertex_shader_source_file;
	std::string const m_fragment_shader_source_file;
	GLuint m_vertex_shader;
	GLuint m_fragment_shader;
	static std::unordered_map<std::string, GLuint> s_compiled_shaders_list;
};
