#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <utility>

namespace fw
{

Model::Model()
{
}

Model::~Model()
{
}

bool Model::loadModel(const std::string& file)
{
	Assimp::Importer importer;
	unsigned int flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace;
	const aiScene* aScene = importer.ReadFile(file, flags);

	if (aScene) {
		if (aScene->mNumMeshes == 0) {
			std::cerr << "WARNING: No mesh found in the model; " << file << "\n";
			return false;
		}

		for (unsigned int meshIndex = 0; meshIndex < aScene->mNumMeshes; ++meshIndex) {
			const aiMesh* aMesh = aScene->mMeshes[meshIndex];
			Mesh mesh;
			for (unsigned int vertexIndex = 0; vertexIndex < aMesh->mNumVertices; ++vertexIndex) {
				mesh.vertices.emplace_back(aMesh->mVertices[vertexIndex].x,
										   aMesh->mVertices[vertexIndex].y,
										   aMesh->mVertices[vertexIndex].z);

				mesh.normals.emplace_back(aMesh->mNormals[vertexIndex].x,
										  aMesh->mNormals[vertexIndex].y,
										  aMesh->mNormals[vertexIndex].z);

				if (aMesh->HasTangentsAndBitangents()) {
					mesh.tangents.emplace_back(aMesh->mTangents[vertexIndex].x,
											   aMesh->mTangents[vertexIndex].y,
											   aMesh->mTangents[vertexIndex].z);
				}

				if (aMesh->HasTextureCoords(0)) {
					mesh.uvs.emplace_back(aMesh->mTextureCoords[0][vertexIndex].x,
										  -aMesh->mTextureCoords[0][vertexIndex].y);
				}
			}

			for (unsigned int faceIndex = 0; faceIndex < aMesh->mNumFaces; ++faceIndex) {
				if (aMesh->mFaces[faceIndex].mNumIndices != 3) {
					std::cerr << "ERROR: Unable to parse model indices for " << file << "\n";
					return false;
				}
				mesh.indices.push_back(aMesh->mFaces[faceIndex].mIndices[0]);
				mesh.indices.push_back(aMesh->mFaces[faceIndex].mIndices[1]);
				mesh.indices.push_back(aMesh->mFaces[faceIndex].mIndices[2]);
			}

			aiMaterial* aMaterial = aScene->mMaterials[aMesh->mMaterialIndex];
			if (aMaterial) {
				for (int typeIndex = 0; typeIndex < aiTextureType_UNKNOWN; ++typeIndex) {
					aiTextureType type = static_cast<aiTextureType>(typeIndex);
					unsigned int numTextures = aMaterial->GetTextureCount(type);
					for (unsigned int texIndex = 0; texIndex < numTextures; ++texIndex) {
						aiString path;
						aMaterial->GetTexture(type, 0, &path);
						mesh.materials[type].emplace_back(std::string(path.C_Str()));
					}
				}
			}

			if (mesh.vertices.empty()) {
				std::cerr << "ERROR: Invalid mesh, 0 vertices\n";
				return false;
			}
			meshes.push_back(std::move(mesh));
		}
	} else {
		std::cerr << "WARNING: Failed to read model: " << file << "\n";
		std::cerr << importer.GetErrorString() << "\n";
		return false;
	}

	if (meshes.empty()) {
		std::cerr << "WARNING: Empty model\n";
		return false;
	}
	return true;
}

const Model::Meshes& Model::getMeshes() const
{
	return meshes;
}

} // fw
