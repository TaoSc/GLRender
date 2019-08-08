#include "Shader.h"

#include <sstream>

#include <glm/gtc/type_ptr.hpp>


std::unordered_map<std::string, GLuint> Shader::s_compiled_shaders_list = {};

Shader::Shader(std::string const& vertex_shader_source_file, std::string const& fragment_shader_source_file) : m_shader_program_id(0),
m_vertex_shader_source_file(vertex_shader_source_file),
m_fragment_shader_source_file(fragment_shader_source_file)
{
	m_vertex_shader = (s_compiled_shaders_list.count(vertex_shader_source_file)) ? s_compiled_shaders_list[vertex_shader_source_file] : compile(vertex_shader_source_file, GL_VERTEX_SHADER);
	m_fragment_shader = (s_compiled_shaders_list.count(fragment_shader_source_file)) ? s_compiled_shaders_list[fragment_shader_source_file] : compile(fragment_shader_source_file, GL_FRAGMENT_SHADER);

	link();
}

Shader::~Shader()
{
	s_compiled_shaders_list.erase(m_vertex_shader_source_file);
	glDeleteShader(m_vertex_shader);
	s_compiled_shaders_list.erase(m_fragment_shader_source_file);
	glDeleteShader(m_fragment_shader);

	glDeleteProgram(m_shader_program_id);
}


// Returns the shader program ID
GLuint Shader::id() const
{
	return m_shader_program_id;
}


void Shader::setUni(std::string const& name, bool const& value) const
{
	glUniform1i(glGetUniformLocation(m_shader_program_id, name.c_str()), static_cast<GLboolean>(value));
}
void Shader::setUni(std::string const& name, int const& value) const
{
	glUniform1i(glGetUniformLocation(m_shader_program_id, name.c_str()), static_cast<GLint>(value));
}
void Shader::setUni(std::string const& name, float const& value) const
{
	glUniform1f(glGetUniformLocation(m_shader_program_id, name.c_str()), static_cast<GLfloat>(value));
}
void Shader::setUni(std::string const& name, glm::mat4 const& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_shader_program_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setUni(std::string const& name, float const& value1, float const& value2, float const& value3) const
{
	glUniform3f(glGetUniformLocation(m_shader_program_id, name.c_str()), static_cast<GLfloat>(value1), static_cast<GLfloat>(value2), static_cast<GLfloat>(value3));
}
void Shader::setUni(std::string const& name, glm::vec3 const& value) const
{
	glUniform3fv(glGetUniformLocation(m_shader_program_id, name.c_str()), 1, glm::value_ptr(value));
}


GLuint Shader::compile(std::string const& file_path, GLuint const& type)
{
	std::cout << "Compiling shader \"" << file_path << "\" (" << type << ")." << std::endl;
	GLuint shader_id(glCreateShader(type));
	if (shader_id == 0) {
		std::cerr << "Error: shader type (" << type << ") does not exist." << std::endl;
		return 0;
	}


	std::ifstream file(file_path);
	std::string file_contents;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		std::stringstream file_stream;
		file_stream << file.rdbuf();
		file.close();
		file_contents = file_stream.str();
	}
	catch (std::ifstream::failure)
	{
		std::cerr << "Error: shader file could not be read successfully: " << std::endl;
	}
	const GLchar* source_code_string(file_contents.c_str());


	glShaderSource(shader_id, 1, &source_code_string, nullptr);
	glCompileShader(shader_id);


	GLint success(0);
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

	if (success != GL_TRUE) {
		GLint error_size(0);
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &error_size);

		GLchar* error(new char[static_cast<size_t>(error_size) + 1]);

		glGetShaderInfoLog(shader_id, error_size, nullptr, error);
		error[error_size] = '\0';

		std::cerr << "Shader compilation failed (file: \"" << file_path << "\"): " << error << "." << std::endl;

		delete[] error;
		glDeleteShader(shader_id);
		return 0;
	}

	s_compiled_shaders_list[file_path] = shader_id;

	return shader_id;
}

void Shader::link()
{
	std::cout << "Linking..." << std::endl;

	// Unrelated; just to test if the list behaves properly
	std::cout << "----" << std::endl << "Compiled shaders list:" << std::endl;
	for (auto const& element : s_compiled_shaders_list)
		std::cout << element.first << " (id: " << element.second << ")" << std::endl;
	std::cout << "----" << std::endl;


	if (m_vertex_shader == 0 || m_fragment_shader == 0) {
		std::cerr << "At least one shader didn't compile. Linking aborted." << std::endl;
		return;
	}

	m_shader_program_id = glCreateProgram();

	glAttachShader(m_shader_program_id, m_vertex_shader);
	glAttachShader(m_shader_program_id, m_fragment_shader);

	glLinkProgram(m_shader_program_id);


	glDeleteShader(m_vertex_shader);
	glDeleteShader(m_fragment_shader);


	GLint success(0);
	glGetProgramiv(m_shader_program_id, GL_LINK_STATUS, &success);

	if (success != GL_TRUE) {
		GLint error_size = 0;
		glGetProgramiv(m_shader_program_id, GL_INFO_LOG_LENGTH, &error_size);

		GLchar* error(new char[static_cast<size_t>(error_size) + 1]);

		glGetProgramInfoLog(m_shader_program_id, error_size, nullptr, error);
		error[error_size] = '\0';

		std::cerr << "Shader linking error: " << error << "." << std::endl;

		delete[] error;
		glDeleteProgram(m_shader_program_id);
		return;
	}

	std::cout << "Link succeeded (id: " << m_shader_program_id << ")." << std::endl;
}
