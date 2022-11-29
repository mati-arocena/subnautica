#include "Model.h"
#include <assimp/Importer.hpp>
#include "Utils.h"



#include "GameInstance.h"
#include "Definitions.h"
#include "Animator.h"


std::map<std::string,BoneInfo>& Model::getBoneInfoMap()
{
	return this->m_BoneInfoMap;
}

int& Model::getBoneCount()
{
	return m_BoneCounter;
}

void Model::setIsAnimation(bool isAnimation)
{
	for (auto &mesh : meshes)
	{
		mesh.setHasAnimation(isAnimation);
	}
}

Model::Model(std::string path) : GameObject()
{
	loadModel(path);
}

Model::~Model()
{
}

void Model::render()
{
	
	for (Mesh mesh : meshes)
	{
		mesh.render();
	}
}

void Model::update(double DeltaTime)
{
	// TODO: Hacer
}

void Model::loadModel(std::string path)
{
	Assimp::Importer import;
	import.SetPropertyFloat("PP_GSN_MAX_SMOOTHING_ANGLE", 90);

	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | 
		aiProcess_FlipUVs | 
		aiProcess_GenSmoothNormals | 
		aiProcess_JoinIdenticalVertices | 
		aiProcess_CalcTangentSpace |
		aiProcess_GenBoundingBoxes);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)

	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	// Initial trnasformation
	glm::mat4 transformMat = glm::identity<glm::mat4>();

	processNode(scene->mRootNode, scene, transformMat);
}

void Model::processNode(aiNode* node, const aiScene* scene, glm::mat4 transformMat)
{
	// Combined transformations applied
	const glm::mat4 node_transformMat = transformMat * convertMatrix(node->mTransformation);
	glm::vec3 pos;
	glm::quat rot;

	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh m = processMesh(mesh, scene, node_transformMat);

		aiVector3D max = mesh->mAABB.mMax;
		aiVector3D min = mesh->mAABB.mMin;
		/*
		btVector3 halfExtent(std::abs(max.x - min.x) / 2, std::abs(max.y - min.y) / 2, std::abs(max.z - min.z) / 2);
		btCollisionShape* boxCollisionShape = new btBoxShape(halfExtent);
		btMotionState* motionState = new btDefaultMotionState(btTransform(
			btQuaternion(rot.x, rot.y, rot.z, rot.w),
			btVector3(pos.x, pos.y, pos.z)
		));
		btTransform transform;
		transform.setIdentity();
		btDefaultMotionState* motionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo Info(
			0,
			motionState,
			boxCollisionShape,
			btVector3(0,0,0)
		);

		std::shared_ptr<btRigidBody> rigidBody = std::make_shared<btRigidBody>(Info);

		this->collisionObjects.push_back(rigidBody);
		*/

		meshes.push_back(processMesh(mesh, scene, node_transformMat));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, node_transformMat);
	}
}

void setVertexBoneData(Vertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
	{
		if (vertex.m_BoneIDs[i] < 0)
		{
			vertex.m_Weights[i] = weight;
			vertex.m_BoneIDs[i] = boneID;
			break;
		}
	}
}

void Model::extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = m_BoneCounter;
			newBoneInfo.offset = convertMatrix(mesh->mBones[boneIndex]->mOffsetMatrix);
			m_BoneInfoMap[boneName] = newBoneInfo;
			boneID = m_BoneCounter;
			m_BoneCounter++;
		}
		else
		{
			boneID = m_BoneInfoMap[boneName].id;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			setVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 transformMat)
{
	std::vector<Vertex> verticesLOD0;
	std::vector<unsigned int> indicesLOD0;
	std::vector<Texture*> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// process vertex positions, normals and texture coordinates
		vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		
		vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		vertex.Bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

		verticesLOD0.push_back(vertex);
	}

	for (int i = 0; i < static_cast<int>(mesh->mNumFaces); i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indicesLOD0.push_back(face.mIndices[j]);
	}

	extractBoneWeightForVertices(verticesLOD0, mesh, scene);

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN


	glm::vec3 diffuseColor, specularColor;
	diffuseColor = specularColor = glm::vec3(0);
	float specularStrenght, specularExponent;
	specularStrenght = specularExponent = 0;

	// 2. specular maps
	std::vector<Texture*> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TEXTURE_SPECULAR);
	if (specularMaps.size() == 0)
	{
		aiColor3D aiSpecCol;
		// material->Get(AI_MATKEY_COLOR_SPECULAR, aiSpecCol); No anda
		material->Get(AI_MATKEY_SHININESS, specularExponent);
		// material->Get(AI_MATKEY_SHININESS_STRENGTH, specularStrenght);
		specularStrenght = 1.0;
		specularColor = glm::vec3(1.);
	}
	else
	{
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	// 1. diffuse maps
	std::vector<Texture*> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TEXTURE_DIFFUSE);
	if (diffuseMaps.size() == 0)
	{	
		aiColor3D aiDiff;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, aiDiff);
		diffuseColor = toVec3(aiDiff);
	}
	else
	{
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	}
	
	// 3. normal maps
	std::vector<Texture*> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, TEXTURE_NORMAL);
	if (normalMaps.size() == 0)
	{
		normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, TEXTURE_NORMAL);
	}

	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture*> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	Material *m = new Material(textures, GameInstance::getInstance().getShader(NORMAL_SHADER), diffuseColor, specularColor, specularStrenght, specularExponent);
	
	glm::vec3 AABBmin = { mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z };
	glm::vec3 AABBmax = { mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z };
	return Mesh(verticesLOD0, indicesLOD0, verticesLOD0, indicesLOD0, verticesLOD0, indicesLOD0, m, transformMat, AABBmin, AABBmax);
}

std::vector<Texture*> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture*> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		std::string filename;
		filename = this->directory + '/' + str.C_Str();
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (filename == textures_loaded[j]->path)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture* texture = new Texture(filename.c_str(), typeName, true, true);
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}

	}
	return textures;

}

void Model::clipModel(glm::vec4 plane)
{
	for (Mesh &m : meshes)
	{
		m.setClipPlane(plane);
	}
}

void Model::render_withShader(std::shared_ptr<Shader> shader)
{
	for (Mesh mesh : meshes)
	{
		mesh.render_withShader(shader);
	}
}

void Model::renderOclussion()
{
	printf("No esta hecho"); // Ya se que es una cagada esto 
}



void Model::move(glm::vec3 movement)
{
	for (auto mesh : meshes)
	{
		mesh.move(movement);
	}
}

void Model::rotate(glm::vec3 rotationAxis, float angle)
{
	for (auto mesh : meshes)
	{
		mesh.rotate(rotationAxis, angle);
	}
}
