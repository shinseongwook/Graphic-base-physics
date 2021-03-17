#pragma once

#include "Mesh.h"
#include "Graphic.h"
#include <vector>

struct BSPTreeNode;

struct BSPTree
{
	BSPTree()
	{
	}
	void Compute_BSPTree();

	void Draw_BSPTree(int height);

	BSPTreeNode* head_node = nullptr;

	int longest_height = 1;
};


struct BSPTreeNode
{
	BSPTreeNode()
	{
	}

	void Compute_BSPTreeNode();
	glm::vec3 Get_Intersect_Point(glm::vec3& p1, glm::vec3& p2, int plane);

	void Draw_BSPTree(int height);

	glm::vec3 median_point = glm::vec3(0.f);

	glm::vec3 min_point = glm::vec3(1.f);
	glm::vec3 max_point = glm::vec3(-1.f);

	std::vector<glm::vec3> containing_vertices;

	unsigned int height = 1;

	Mesh triangles_mesh;

	BSPTreeNode* front= nullptr;
	BSPTreeNode* back = nullptr;
};