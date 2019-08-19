#include "Mesh.h"

Mesh::Mesh(std::vector<VertexStruct> const& vertices, std::vector<unsigned int> const& indices, std::vector<Texture *> const& textures) : m_vertices(vertices), m_indices(indices), m_textures(textures)
{
	setupMesh();
}

void Mesh::Draw(Shader const& shader, bool const& textures) const
{
	if (textures) {
		unsigned int diffuse_nb = 1, specular_nb = 1;
		for (size_t i = 0; i < m_textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(i));

			unsigned int number;
			std::string name = m_textures[i]->type();
			if (name == "diffuse")
				number = diffuse_nb++;
			else if (name == "specular")
				number = specular_nb++;

			shader.setUni(("material." + name/* + std::to_string(number) */), static_cast<int>(i));
			glBindTexture(GL_TEXTURE_2D, m_textures[i]->id());
		}
		glActiveTexture(GL_TEXTURE0);
	}


	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


void Mesh::setupMesh()
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VertexStruct), m_vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(VERTEX_POS_ATTR, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStruct), reinterpret_cast<GLvoid*>(offsetof(VertexStruct, position)));
	glEnableVertexAttribArray(VERTEX_POS_ATTR);
	glVertexAttribPointer(VERTEX_NORMAL_ATTR, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStruct), reinterpret_cast<GLvoid*>(offsetof(VertexStruct, normal)));
	glEnableVertexAttribArray(VERTEX_NORMAL_ATTR);
	glVertexAttribPointer(VERTEX_TEX_ATTR, 2, GL_FLOAT, GL_FALSE, sizeof(VertexStruct), reinterpret_cast<GLvoid*>(offsetof(VertexStruct, tex_coords)));
	glEnableVertexAttribArray(VERTEX_TEX_ATTR);

	glBindVertexArray(0);
}
