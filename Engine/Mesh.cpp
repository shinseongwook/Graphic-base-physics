/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.cpp
Purpose: Implementation of Input class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS300_1
Author: Seongwook,Shin    s.shin  180003319
Creation date: 09/24/2019
End Header --------------------------------------------------------*/ 
#include "Mesh.h"
#include <string>
#include <iostream>
#include <algorithm>
#include "GL/glew.h"
#include "glm/gtc/epsilon.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "BoundingVolume.h"

using namespace glm;

float Mesh::min_plant[3] = { FLT_MAX,FLT_MAX ,FLT_MAX };
float Mesh::max_plant[3] = { -FLT_MAX,-FLT_MAX ,-FLT_MAX };

Mesh::Mesh():RenderType(RENDER_TYPE::TRIANGLE)
{
	translate = {0,0,0};
	scale = { 1.f,1.f,1.f };
	rotate=0.f;
}

void Mesh::MakeCircle()
{
	for (int i = 0; i < 360; ++i)
	{
		Vertices.push_back(vec3(cos(radians((float)i)), 0, sin(radians((float)i))));
		face_index.push_back(i);
		face_index.push_back(i+1);
	}
	face_index.pop_back();
	face_index.push_back(0);

    RenderType = RENDER_TYPE::LINES;
}

void Mesh::MakeQuad()
{
	Vertices.push_back(vec3(1.0f, 0, 1.0f));
	Vertices.push_back(vec3(-1.0f, 0, 1.0f));
	Vertices.push_back(vec3(-1.0f, 0, -1.0f));
	Vertices.push_back(vec3(1.0f, 0, -1.0f));
	face_index.push_back(2);
	face_index.push_back(1);
	face_index.push_back(0);
	face_index.push_back(0);
	face_index.push_back(3);
	face_index.push_back(2);

	//making face normal for each face
	int vertice_size = static_cast<int>(Vertices.size());
	int face_indices_size = static_cast<int>(face_index.size());

	std::vector<std::vector<vec3>> index_face_normal;

	index_face_normal.resize(vertice_size);
	Face_Normal.resize(face_indices_size / 3 * 2);

	for (int index_number = 0; index_number < face_indices_size; index_number += 3)
	{
		vec3 face_center = Vertices[face_index[index_number]] + Vertices[face_index[index_number + 1]] + Vertices[face_index[index_number + 2]];
		face_center /= 3.f;
		vec3 face_nor = glm::cross(Vertices[face_index[index_number]] - Vertices[face_index[index_number + 1]],
			Vertices[face_index[index_number]] - Vertices[face_index[index_number + 2]]);
		face_nor = glm::normalize(face_nor);
		Face_Normal[index_number / 3] = face_nor;
		//center of face
		index_face_normal[face_index[index_number]].push_back(face_nor);
		index_face_normal[face_index[index_number + 1]].push_back(face_nor);
		index_face_normal[face_index[index_number + 2]].push_back(face_nor);
	}

	for (int vertex_number = 0; vertex_number < vertice_size; ++vertex_number)
	{
		for (int i = 0; i < index_face_normal[vertex_number].size(); ++i)
		{
			for (std::vector<glm::vec3>::size_type j = i + (size_t)1; j < index_face_normal[vertex_number].size();)
			{
				float result = glm::dot(index_face_normal[vertex_number][i], index_face_normal[vertex_number][j]) - 1.f;
				if (abs(result) <= epsilon<float>())
				{
					index_face_normal[vertex_number].erase(index_face_normal[vertex_number].begin() + j);
				}
				else
				{
					++j;
				}
			}
		}
		int normal_size = static_cast<int>(index_face_normal[vertex_number].size());
		glm::vec3 vertex_normal = { 0,0,0 };
		for (int normal_number = 0; normal_number < normal_size; ++normal_number)
		{
			vertex_normal += index_face_normal[vertex_number][normal_number];
		}
		vertex_normal /= normal_size;
		Vertex_Normal.emplace_back(vertex_normal);
	}

	RenderType = RENDER_TYPE::TRIANGLE;
	SetNeedCalculateUV(true);
}

void Mesh::MakeSphere(float radius, int numDivisions)
{
	Clear_Mesh();
	RenderType = RENDER_TYPE::TRIANGLE;
	int sectorCount = 360 / numDivisions;
	int stackCount = 360 / numDivisions;

	float piValue = pi<float>();
	float xy;
	float sectorStep = 2 * piValue / sectorCount;
	float stackStep = piValue / stackCount;
	float sectorAngle, stackAngle;

	vec3 sphere_pos;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = piValue / 2 - i * stackStep;
		xy = radius * cos(stackAngle);
		sphere_pos.z = radius * sin(stackAngle);

		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;

			sphere_pos.x = xy * cos(sectorAngle);
			sphere_pos.y = xy * sin(sectorAngle);
			Vertices.push_back(sphere_pos);
		}
	}

	int k1, k2;
	for (int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);
		k2 = k1 + sectorCount + 1;

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			if (i != 0)
			{
				face_index.push_back(k1);
				face_index.push_back(k2);
				face_index.push_back(k1+1);
			}
			if (i != (stackCount - 1))
			{
				face_index.push_back(k1 + 1);
				face_index.push_back(k2);
				face_index.push_back(k2 + 1);
			}
		}
	}
	//making face normal for each face
	int vertice_size = static_cast<int>(Vertices.size());
	int face_indices_size = static_cast<int>(face_index.size());

	std::vector<std::vector<vec3>> index_face_normal;

	index_face_normal.resize(vertice_size);
	Face_Normal.resize(face_indices_size / 3 * 2);

	for (int index_number = 0; index_number < face_indices_size; index_number += 3)
	{
		vec3 face_center = Vertices[face_index[index_number]] + Vertices[face_index[index_number + 1]] + Vertices[face_index[index_number + 2]];
		face_center /= 3.f;
		vec3 face_nor = glm::cross(Vertices[face_index[index_number]] - Vertices[face_index[index_number + 1]],
			Vertices[face_index[index_number]] - Vertices[face_index[index_number + 2]]);
		face_nor = glm::normalize(face_nor);
		Face_Normal[index_number / 3] = face_nor;
		//center of face
		index_face_normal[face_index[index_number]].push_back(face_nor);
		index_face_normal[face_index[index_number + 1]].push_back(face_nor);
		index_face_normal[face_index[index_number + 2]].push_back(face_nor);
	}

	for (int vertex_number = 0; vertex_number < vertice_size; ++vertex_number)
	{
		for (int i = 0; i < index_face_normal[vertex_number].size(); ++i)
		{
			for (std::vector<glm::vec3>::size_type j = i + (size_t)1; j < index_face_normal[vertex_number].size();)
			{
				float result = glm::dot(index_face_normal[vertex_number][i], index_face_normal[vertex_number][j]) - 1.f;
				if (abs(result) <= epsilon<float>())
				{
					index_face_normal[vertex_number].erase(index_face_normal[vertex_number].begin() + j);
				}
				else
				{
					++j;
				}
			}
		}
		int normal_size = static_cast<int>(index_face_normal[vertex_number].size());
		glm::vec3 vertex_normal = { 0,0,0 };
		for (int normal_number = 0; normal_number < normal_size; ++normal_number)
		{
			vertex_normal += index_face_normal[vertex_number][normal_number];
		}
		vertex_normal /= normal_size;
		Vertex_Normal.emplace_back(vertex_normal);
	}
	SetNeedCalculateUV(true);
}

void Mesh::MakeAABB(glm::vec3 min, glm::vec3 max)
{
	vec3 left_bottom_front = vec3(min.x, min.y, min.z);
	vec3 right_bottom_front = vec3(max.x,min.y,min.z);
	vec3 right_top_front = vec3(max.x, max.y, min.z);
	vec3 left_top_front = vec3(min.x, max.y, min.z);

	vec3 left_bottom_back = vec3(min.x, min.y, max.z);
	vec3 right_bottom_back = vec3(max.x, min.y, max.z);
	vec3 right_top_back = vec3(max.x, max.y, max.z);
	vec3 left_top_back = vec3(min.x, max.y, max.z);

	Vertices.push_back(left_bottom_front);
	Vertices.push_back(right_bottom_front);
	Vertices.push_back(right_bottom_front);
	Vertices.push_back(right_top_front);
	Vertices.push_back(right_top_front);
	Vertices.push_back(left_top_front);
	Vertices.push_back(left_top_front);
	Vertices.push_back(left_bottom_front);

	Vertices.push_back(left_bottom_front);
	Vertices.push_back(left_bottom_back);
	Vertices.push_back(right_bottom_front);
	Vertices.push_back(right_bottom_back);
	Vertices.push_back(right_top_front);
	Vertices.push_back(right_top_back);
	Vertices.push_back(left_top_front);
	Vertices.push_back(left_top_back);

	Vertices.push_back(left_bottom_back);
	Vertices.push_back(right_bottom_back);
	Vertices.push_back(right_bottom_back);
	Vertices.push_back(right_top_back);
	Vertices.push_back(right_top_back);
	Vertices.push_back(left_top_back);
	Vertices.push_back(left_top_back);
	Vertices.push_back(left_bottom_back);
}

void Mesh::Clear_Mesh()
{
	Vertices.clear();
	Vertex_Normal_Line.clear();
	Vertex_Normal.clear();
	Face_Normal_Line.clear();
	Face_Normal.clear();
	face_index.clear();
}

void Mesh::Load_Objfile(std::string str)
{
	Clear_Mesh();

	FILE* fp;
	fopen_s(&fp, ("models/" + str+".obj").c_str(), "r");
	if (fp == NULL)
	{
		std::cout << "fail to open " << str << ".obj file.\n";
		return;
	}
	float min[3] = { FLT_MAX,FLT_MAX ,FLT_MAX };
	float max[3] = { -FLT_MAX,-FLT_MAX ,-FLT_MAX };
	bool contain_normal = false;
	bool contain_texture = false;

	bool skip_fscanf = false;
	char lineHeader[256] = {};
	while (true)
	{
		// read the first word of the line
		if (skip_fscanf==false)
		{
			int res = fscanf_s(fp, "%s", lineHeader, 256);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.
		}
		skip_fscanf = false;

		if (strcmp(lineHeader, "v") == 0)
		{
			glm::vec3 vertex;
			int success=fscanf_s(fp, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			for (int i = 0; i < 3; ++i)
			{
				if (min[i] > vertex[i])
				{
					min[i] = vertex[i];
				}
				else if (max[i] < vertex[i])
				{
					max[i] = vertex[i];
				}
			}
			Vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			glm::vec3 vertex_normal;
			int success = fscanf_s(fp, "%f %f %f\n", &vertex_normal.x, &vertex_normal.y, &vertex_normal.z);

			Vertex_Normal.push_back(vertex_normal);
			contain_normal = true;
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			contain_texture = true;
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[12];

			if (contain_texture)
			{
				int temp;
				int matches = fscanf_s(fp, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &temp, &temp, &vertexIndex[1],
																				 &temp, &temp, &vertexIndex[2], &temp, &temp);
				face_index.push_back(vertexIndex[0] - 1);
				face_index.push_back(vertexIndex[1] - 1);
				face_index.push_back(vertexIndex[2] - 1);
			}
			else if (contain_normal)
			{
				int temp;
				int matches = fscanf_s(fp, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &temp, &vertexIndex[1], &temp, &vertexIndex[2], &temp);

				face_index.push_back(vertexIndex[0] - 1);
				face_index.push_back(vertexIndex[1] - 1);
				face_index.push_back(vertexIndex[2] - 1);
				int index = 2;
				while (true)
				{
					bool is_line_done = false;
					std::string save_temp;
					matches = fscanf_s(fp, "%s\n", lineHeader, 256);

					save_temp = lineHeader;
					if (save_temp == "\\par")
					{
						break;
					}
					if (save_temp.find("\\par") != std::string::npos)
					{
						is_line_done = true;
						save_temp.erase(save_temp.find("\\par"));
					}

					save_temp = save_temp.substr(0, save_temp.find("//"));
					vertexIndex[index + 1] = std::stoi(save_temp);

					face_index.push_back(vertexIndex[0] - 1);
					face_index.push_back(vertexIndex[index] - 1);
					face_index.push_back(vertexIndex[index + 1] - 1);
					++index;

					if (is_line_done)
					{
						break;
					}
				}
			}
			else
			{
				int matches = fscanf_s(fp, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);

				face_index.push_back(vertexIndex[0] - 1);
				face_index.push_back(vertexIndex[1] - 1);
				face_index.push_back(vertexIndex[2] - 1);

				int index = 2;
				while (true)
				{
					bool is_line_done = false;
					std::string save_temp;
					matches = fscanf_s(fp, "%s\n", lineHeader, 256);
					if (matches == EOF)
					{
						strcpy_s(lineHeader,"EOF"); // EOF = End Of File. Quit the loop.
						break;
					}
					save_temp = lineHeader;
					if (save_temp == "\\par"||save_temp=="#")
					{
						break;
					}
					else if (save_temp == "f")
					{
						skip_fscanf = true;
						break;
					}
					if (save_temp.find("\\par") != std::string::npos)
					{
						is_line_done = true;
						save_temp.erase(save_temp.find("\\par"));
					}

					vertexIndex[index + 1] = std::stoi(save_temp);

					face_index.push_back(vertexIndex[0] - 1);
					face_index.push_back(vertexIndex[index] - 1);
					face_index.push_back(vertexIndex[index + 1] - 1);
					++index;

					if (is_line_done)
					{
						break;
					}
				}
			}
		}
	}
	float translate[3];
	float scale[3];

	float max_scale = 0;

	for (int i = 0; i < 3; ++i)
	{
		translate[i] = -((min[i] + max[i])/2.f);
		scale[i] = (max[i] - min[i]);

		if (max_scale < scale[i])
		{
			max_scale = scale[i];
		}
	}

	//calibrating x,y,z
	for (auto& vert : Vertices)
	{
		for (int i = 0; i < 3; ++i)
		{
			vert[i] += translate[i];
			vert[i] /= max_scale;
		}
	}
	//making face normal for each face
	int vertice_size=static_cast<int>(Vertices.size());
	int face_indices_size = static_cast<int>(face_index.size());

	std::vector<std::vector<vec3>> index_face_normal;

	index_face_normal.resize(vertice_size);
	Face_Normal_Line.resize(face_indices_size/3*2);
	Face_Normal.resize(face_indices_size / 3 * 2);

	for (int index_number = 0; index_number < face_indices_size; index_number+=3)
	{	
		vec3 face_center = Vertices[face_index[index_number]] + Vertices[face_index[index_number + 1]] + Vertices[face_index[index_number + 2]];
		face_center /= 3.f;
		vec3 face_nor=glm::cross(Vertices[face_index[index_number]] - Vertices[face_index[index_number+1]],
								 Vertices[face_index[index_number]] - Vertices[face_index[index_number+2]]);
		face_nor = glm::normalize(face_nor);
		Face_Normal[index_number / 3]=face_nor;
		Face_Normal_Line[(index_number / 3) * 2] = face_center;
		Face_Normal_Line[(index_number / 3) * 2 + 1] = face_center+face_nor/40.f;
		//center of face
		index_face_normal[face_index[index_number]].push_back(face_nor);
		index_face_normal[face_index[index_number + 1]].push_back(face_nor);
		index_face_normal[face_index[index_number + 2]].push_back(face_nor);
	}
	if (Vertex_Normal.size() != 0)
	{
		Vertex_Normal.clear();
	}
	for (int vertex_number = 0; vertex_number < vertice_size; ++vertex_number)
	{
		for (int i = 0; i < index_face_normal[vertex_number].size(); ++i)
		{
			for (std::vector<glm::vec3>::size_type j = i+ (size_t)1; j < index_face_normal[vertex_number].size();)
			{
				float result = glm::dot(index_face_normal[vertex_number][i], index_face_normal[vertex_number][j])-1.f;
				if (abs(result) <= epsilon<float>())
				{
					index_face_normal[vertex_number].erase(index_face_normal[vertex_number].begin()+j);
				}
				else
				{
					++j;
				}
			}
		}
		int normal_size = static_cast<int>(index_face_normal[vertex_number].size());
		glm::vec3 vertex_normal = { 0,0,0 };
		for (int normal_number = 0; normal_number < normal_size; ++normal_number)
		{
			vertex_normal += index_face_normal[vertex_number][normal_number];
		}
		vertex_normal /= normal_size;
		Vertex_Normal.emplace_back(vertex_normal);
		Vertex_Normal_Line.emplace_back(Vertices[vertex_number]);
		Vertex_Normal_Line.emplace_back(Vertices[vertex_number] +
			glm::normalize(vertex_normal) / 40.f);
	}
	SetNeedCalculateUV(false);
	SetTextureCoord(TEXTURE_MAPPING_TYPE::CUBE_MAPPING, true);
}

void Mesh::Load_Plyfile(std::string str)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(str, aiProcess_Triangulate| aiProcess_JoinIdenticalVertices);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	int prev_index_num = static_cast<int>(Vertices.size());
	int num_vertices = scene->mMeshes[0]->mNumVertices;
	int num_faces= scene->mMeshes[0]->mNumFaces;

	for (int i = 0; i < num_vertices; ++i)
	{
		vec3 vertex_coord=vec3(scene->mMeshes[0]->mVertices[i].x, scene->mMeshes[0]->mVertices[i].y, scene->mMeshes[0]->mVertices[i].z);
		vec3 vertex_normal = vec3(scene->mMeshes[0]->mNormals[i].x, scene->mMeshes[0]->mNormals[i].y, scene->mMeshes[0]->mNormals[i].z);

		for (int i = 0; i < 3; ++i)
		{
			if (min_plant[i] > vertex_coord[i])
			{
				min_plant[i] = vertex_coord[i];
			}
			else if (max_plant[i] < vertex_coord[i])
			{
				max_plant[i] = vertex_coord[i];
			}
		}
		Vertices.emplace_back(vertex_coord);
		Vertex_Normal.emplace_back(vertex_normal);
	}

	for (int i = 0; i < num_faces; ++i)
	{
		face_index.push_back(prev_index_num+scene->mMeshes[0]->mFaces[i].mIndices[0]);
		face_index.push_back(prev_index_num+scene->mMeshes[0]->mFaces[i].mIndices[1]);
		face_index.push_back(prev_index_num+scene->mMeshes[0]->mFaces[i].mIndices[2]);
	}
	
	SetTextureCoord(TEXTURE_MAPPING_TYPE::CUBE_MAPPING, true);
	return;
}

void Mesh::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void Mesh::SetTranslate(glm::vec3 Translate)
{
	translate = Translate;
}

void Mesh::SetScale(glm::vec3 Scale)
{
	scale = Scale;
}

void Mesh::SetRotate(float Rotate)
{
	rotate = Rotate;
}

void Mesh::SetTextureCoord(TEXTURE_MAPPING_TYPE mapping_type, bool is_position)
{
	Texture_Coord.clear();

	for (int nVertex = 0; nVertex < Vertices.size(); ++nVertex)
	{
		glm::vec3 V;
		if (is_position)
		{
			V = GetTRSMatrix()*vec4(Vertices[nVertex], 1.0f);
		}
		else//if normal
		{
			V = GetTRSMatrix()*vec4(Vertex_Normal[nVertex], 1.0f);
		}
		glm::vec2 uv(0.0f);

		if (mapping_type == TEXTURE_MAPPING_TYPE::CUBE_MAPPING)
		{
			float x = V.x;
			float y = V.y;
			float z = V.z;

			float absX = abs(x);
			float absY = abs(y);
			float absZ = abs(z);

			int isXPositive = x > 0 ? 1 : 0;
			int isYPositive = y > 0 ? 1 : 0;
			int isZPositive = z > 0 ? 1 : 0;

			float maxAxis, uc, vc;

			// POSITIVE X
			if (bool(isXPositive) && (absX >= absY) && (absX >= absZ))
			{
				// u (0 to 1) goes from +z to -z
				// v (0 to 1) goes from -y to +y
				maxAxis = absX;
				uc = -z;
				vc = y;
			}

			// NEGATIVE X
			else if (!bool(isXPositive) && absX >= absY && absX >= absZ)
			{
				// u (0 to 1) goes from -z to +z
				// v (0 to 1) goes from -y to +y
				maxAxis = absX;
				uc = z;
				vc = y;
			}

			// POSITIVE Y
			else if (bool(isYPositive) && absY >= absX && absY >= absZ)
			{
				// u (0 to 1) goes from -x to +x
				// v (0 to 1) goes from +z to -z
				maxAxis = absY;
				uc = x;
				vc = -z;
			}

			// NEGATIVE Y
			else if (!bool(isYPositive) && absY >= absX && absY >= absZ)
			{
				// u (0 to 1) goes from -x to +x
				// v (0 to 1) goes from -z to +z
				maxAxis = absY;
				uc = x;
				vc = z;
			}

			// POSITIVE Z
			else if (bool(isZPositive) && absZ >= absX && absZ >= absY)
			{
				// u (0 to 1) goes from -x to +x
				// v (0 to 1) goes from -y to +y
				maxAxis = absZ;
				uc = x;
				vc = y;
			}

			// NEGATIVE Z
			else if (!bool(isZPositive) && absZ >= absX && absZ >= absY)
			{
				// u (0 to 1) goes from +x to -x
				// v (0 to 1) goes from -y to +y
				maxAxis = absZ;
				uc = -x;
				vc = y;
			}

			// Convert range from -1 to 1 to 0 to 1
			uv.s = 0.5f * (uc / maxAxis + 1.0f);
			uv.t = 0.5f * (vc / maxAxis + 1.0f);
		}
		else if (mapping_type == TEXTURE_MAPPING_TYPE::CYLINDRICAL_MAPPING)
		{
			if (is_position)
			{
				V = GetTRSMatrix()*vec4(Vertices[nVertex], 1.0f);
			}
			else//if normal
			{
				V = vec4(Vertex_Normal[nVertex], 1.0f);
			}

			glm::vec3 centroidVec = normalize(V-translate);
			float theta = glm::degrees(atan2(centroidVec.y, centroidVec.x)) + 180.f;
			float z = (centroidVec.z + 1.0f) / 2.f;

			uv.x = theta / 360.f;
			uv.y = z;
		}
		else if (mapping_type == TEXTURE_MAPPING_TYPE::SPHERICAL_MAPPING)
		{
			if (is_position)
			{
				V = GetTRSMatrix()*vec4(Vertices[nVertex], 1.0f);
			}
			else//if normal
			{
				V = vec4(Vertex_Normal[nVertex], 1.0f);
			}

			glm::vec3 centroidVec = normalize(V- translate);
			float theta = glm::degrees(glm::atan(centroidVec.y, centroidVec.x))+180.0f;
			float z = centroidVec.z;
			float phi = glm::degrees(glm::acos(z / centroidVec.length()));

			uv.x = theta / 360.0f;
			uv.y = (1.0f - (phi / 180.0f)) * 2.f;
		}
		Texture_Coord.push_back(uv);
	}

}

void Mesh::SetNeedCalculateUV(bool tf)
{
	need_calculate_uv = tf;
}

glm::vec3 Mesh::GetTranslate()
{
	return translate;
}

std::vector<vec3>& Mesh::GetVertices()
{
	return Vertices;
}

std::vector<glm::vec3>& Mesh::GetVertexNormalLine()
{
	return Vertex_Normal_Line;
}

std::vector<glm::vec3>& Mesh::GetVertexNormal()
{
	return Vertex_Normal;
}

std::vector<glm::vec3>& Mesh::GetFaceNormalLine()
{
	return Face_Normal_Line;
}

std::vector<glm::vec3>& Mesh::GetFaceNormal()
{
	return Face_Normal;
}

std::vector<glm::vec2>& Mesh::GetTextureCoord()
{
	return Texture_Coord;
}

glm::mat4x4 Mesh::GetTRSMatrix()
{
	return glm::translate(translate) *
			glm::rotate(rotate, glm::vec3(0.0f, 0.0f, 1.0f))*
		   glm::scale(scale);
}

glm::mat4x4 Mesh::GetTRSMatrix_PCA()
{
	return glm::translate(translate) *
		rotate_matrix*
		glm::scale(scale);
}

std::vector<unsigned int>& Mesh::GetIndices()
{
	return face_index;
}

int Mesh::GetVerticesSize() const
{
    return (int)Vertices.size();
}

int Mesh::GetVertexNormalLineSize() const
{
	return (int)Vertex_Normal_Line.size();
}

int Mesh::GetVertexNormalSize() const
{
	return (int)Vertex_Normal.size();
}

int Mesh::GetFaceNormalLineSize() const
{
	return (int)Face_Normal_Line.size();
}

int Mesh::GetFaceNormalSize() const
{
	return (int)Face_Normal.size();
}

int Mesh::GetTextureCoordSize() const
{
	return (int)Texture_Coord.size();
}

RENDER_TYPE Mesh::GetRenderType() const
{
    return RenderType;
}

bool Mesh::GetNeedCalculateUV() const
{
	return need_calculate_uv;
}

void Mesh::Calibrating_Vertexcoord()
{
	float translate[3];
	float scale[3];

	float max_scale = 0;

	for (int i = 0; i < 3; ++i)
	{
		translate[i] = -((min_plant[i] + max_plant[i]) / 2.f);
		scale[i] = (max_plant[i] - min_plant[i]);

		if (max_scale < scale[i])
		{
			max_scale = scale[i];
		}
	}

	//calibrating x,y,z
	for (auto& vert : Vertices)
	{
		for (int i = 0; i < 3; ++i)
		{
			vert[i] += translate[i];
			vert[i] /= max_scale;
		}
	}

	int face_indices_size = face_index.size();
	int vertice_size = Vertices.size();

	Face_Normal.resize(face_indices_size / 3);
	Face_Normal_Line.resize(face_indices_size / 3 * 2);
	Vertex_Normal_Line.resize(vertice_size * 2);

	for (int index_number = 0; index_number < face_indices_size; index_number += 3)
	{
		vec3 face_center = Vertices[face_index[index_number]] + Vertices[face_index[index_number + 1]] + Vertices[face_index[index_number + 2]];
		face_center /= 3.f;
		vec3 face_nor = glm::cross(Vertices[face_index[index_number]] - Vertices[face_index[index_number + 1]],
			Vertices[face_index[index_number]] - Vertices[face_index[index_number + 2]]);
		face_nor = glm::normalize(face_nor);
		Face_Normal[index_number / 3] = face_nor;
		Face_Normal_Line[(index_number / 3) * 2] = face_center;
		Face_Normal_Line[(index_number / 3) * 2 + 1] = face_center + face_nor / 100.f;
	}

	for (int vertex_number = 0; vertex_number < vertice_size; ++vertex_number)
	{
		Vertex_Normal_Line.emplace_back(Vertices[vertex_number]);
		Vertex_Normal_Line.emplace_back(Vertices[vertex_number] + glm::normalize(Vertex_Normal[vertex_number]) / 100.f);
	}
}

void Mesh::Compute_BoundingVolume()
{
	bounding_volume = new BoundingVolume();
	bounding_volume->owner = this;
	bounding_volume->containing_vertices = Vertices;
	bounding_volume->ComputeBoundingVolume();

}
