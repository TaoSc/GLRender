#pragma once

#include <string>
#include <vector>

#include <glm\common.hpp>

#include "Shader.h"
#include "Texture.h"


struct VertexStruct {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coords;
};

constexpr GLuint VERTEX_POS_ATTR = 0, VERTEX_NORMAL_ATTR = 1, VERTEX_TEX_ATTR = 2;


class Mesh {
public:
	Mesh(std::vector<VertexStruct> const& vertices, std::vector<unsigned int> const& indices, std::vector<Texture *> const& textures);
	void Draw(Shader const& shader, bool const& textures = true) const;

private:
	std::vector<VertexStruct> const m_vertices;
	std::vector<GLuint> const m_indices;
	std::vector<Texture *> const m_textures;

	unsigned int m_vao, m_vbo, m_ebo;

	void setupMesh();
};
