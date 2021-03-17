#pragma once

#include "Mesh.h"
#include "Graphic.h"
#include <vector>

struct OctreeNode;

struct Octree
{
	Octree()
	{
	}
	void Compute_Octree();

	void Draw_Octree(int height);

	OctreeNode* head_node = nullptr;

	int longest_height = 1;
};


struct OctreeNode
{
	OctreeNode()
	{
		for (int i = 0; i < 8; ++i)
		{
			child[i] = nullptr;
		}
	}

	void Compute_OctreeNode();

	int Judge_quadrant(glm::vec3& vec);


	void Draw_Octree(int height);

	glm::vec3 center_point = glm::vec3(0.f);

	glm::vec3 min_point = glm::vec3(-1.f);
	glm::vec3 max_point = glm::vec3(1.f);

	std::vector<glm::vec3> containing_vertices;

	unsigned int height = 1;

	Mesh aabb_mesh;
	Mesh triangles_mesh;

	OctreeNode* child[8] = { nullptr };
};