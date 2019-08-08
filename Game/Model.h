#pragma once

#include <string>
#include <vector>

#include <assimp/scene.h>

#include "Mesh.h"
#include "Shader.h"


class Model
{
public:
	explicit Model(std::string const& path, GLuint const& texture_wrapping = GL_REPEAT) : m_directory(path.substr(0, path.find_last_of('/')) + "/")
	{
		loadModel(path, texture_wrapping);
	}

	void Draw(Shader const& shader, bool const& textures = true) const;

private:
	std::vector<Mesh> m_meshes;
	std::vector<Texture> m_textures_loaded;
	std::string const m_directory;

	void loadModel(std::string const& path, GLuint const& texture_wrapping);
	void processNode(aiNode* const& node, const aiScene* scene, GLuint const& texture_wrapping);
	Mesh processMesh(aiMesh* const& mesh, const aiScene* scene, GLuint const& texture_wrapping);
	std::vector<Texture> loadMaterialTextures(aiMaterial* const& material, aiTextureType const& type, std::string const& type_name, GLuint const& texture_wrapping);
};