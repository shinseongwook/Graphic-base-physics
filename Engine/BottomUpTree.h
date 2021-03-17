/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BottomUpTree.h
Purpose: Definition of BottomUpTree class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS350_2
Author: Seongwook,Shin    s.shin  180003319
Creation date: 03/08/2020
End Header --------------------------------------------------------*/ 
#pragma once
#include "Mesh.h"
#include "Graphic.h"
#include <vector>

struct BottomUpNode;

struct BottomUpTree
{
	void Compute_AABB();
	void Compute_BS();

	void Find_Nearest_Object_Indices_AABB(int& i, int& j);
	void Find_Nearest_Object_Indices_BS(int& i, int& j);

	void Draw_AABB(int height);
	void Draw_Sphere(int height);

	std::vector<BottomUpNode*>node_vector;
	BottomUpNode* head_node=nullptr;

	int longest_height = 1;
};


struct BottomUpNode
{
	BottomUpNode()
	{
		sphere_mesh.MakeSphere(1.f, 15);
	}

	void Make_AABB();
	void Make_BS();

	void Check_MinMax(glm::vec3 vertex);

	int ComputeHeight();

	void Draw_AABB(int height);
	void Draw_Bounding_Sphere(int height);

	glm::vec3 median_point = glm::vec3(0.f);
	float radius = 0.f;	//for bounding sphere
	glm::vec3 min_point = glm::vec3(0.f);
	glm::vec3 max_point = glm::vec3(0.f);

	std::vector<glm::vec3> containing_vertices;

	unsigned int height = 1;

	Mesh sphere_mesh;
	Mesh aabb_mesh;

	BottomUpNode* left = nullptr;
	BottomUpNode* right = nullptr;
};