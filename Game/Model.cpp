#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Texture.h"


void Model::Draw(Shader const& shader, bool const& textures) const
{
	for (Mesh const& mesh : m_meshes)
		mesh.Draw(shader, textures);
}

void Model::loadModel(std::string const& path, GLuint const& texture_wrapping)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Error when loading model: " << importer.GetErrorString() << std::endl;
		return;
	}

	processNode(scene->mRootNode, scene, texture_wrapping);
}

void Model::processNode(aiNode* const& node, const aiScene* scene, GLuint const& texture_wrapping)
{
	for (size_t i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(processMesh(mesh, scene, texture_wrapping));
	}

	for (size_t i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene, texture_wrapping);
	}
}

Mesh Model::processMesh(aiMesh* const& mesh, const aiScene* scene, GLuint const& texture_wrapping)
{
	std::vector<VertexStruct> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture *> textures;


	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		VertexStruct vertex;
		glm::vec3 vector;

		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		if (mesh->mTextureCoords[0]) {
			glm::vec2 vector_2d;
			vector_2d.x = mesh->mTextureCoords[0][i].x;
			vector_2d.y = mesh->mTextureCoords[0][i].y;
			vertex.tex_coords = vector_2d;
		}
		else
			vertex.tex_coords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}


	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}


	if (mesh->mMaterialIndex > 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture *> const& diffuse_maps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse", texture_wrapping);
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

		std::vector<Texture *> const& specular_maps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular", texture_wrapping);
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture *> Model::loadMaterialTextures(aiMaterial* const& material, aiTextureType const& type, std::string const& type_name, GLuint const& texture_wrapping)
{
	std::vector<Texture *> textures;

	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString path;
		material->GetTexture(type, i, &path);
		std::string file_loc = m_directory + path.C_Str();

		bool skip = false;
		for (Texture * const& texture_loaded : m_textures_loaded) {
			if (std::strcmp(texture_loaded->path().c_str(), file_loc.c_str()) == 0) {
				textures.push_back(texture_loaded);
				skip = true;
				break;
			}
		}

		if (!skip) {

			Texture *texture = new Texture(file_loc);

			if (!texture->load(texture_wrapping))
				std::cout << "Texture \"" << file_loc << "\" failed loading." << std::endl;

			//std::cout << texture.id() << " - " << type_name << " - " << file_loc << std::endl;

			textures.push_back(texture);
			m_textures_loaded.push_back(texture);
		}
	}

	return textures;
}
