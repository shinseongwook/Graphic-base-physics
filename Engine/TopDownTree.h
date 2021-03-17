/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: TopDownTree.h
Purpose: Definition of TopDownTree class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS350_2
Author: Seongwook,Shin    s.shin  180003319
Creation date: 03/08/2020
End Header --------------------------------------------------------*/ 
#pragma once
#include "Mesh.h"
#include "Graphic.h"

struct TopDownNode;

enum class SPLIT_PLANE { X_PLANE=0,Y_PLANE=1,Z_PLANE=2 };

struct TopDownTree
{
	void Compute_AABB();
	void Compute_BS();

	void Draw_AABB(int height);
	void Draw_Sphere(int height);

	TopDownNode* head_node = nullptr;
};


struct TopDownNode
{
	TopDownNode()
	{
		sphere_mesh.MakeSphere(1.f,40);
	}

	void Compute_HeadAABB();
	void Compute_HeadBS();


	void Compute_TopDownAABB();
	void Compute_TopDownBS();
	void Check_MinMax(glm::vec3 vertex);
	void Compute_Radius();
	float Compute_AABB_Volume();
	float Compute_BS_Volume();

	void Draw_AABB(int height);
	void Draw_Bounding_Sphere(int height);

	SPLIT_PLANE plane=SPLIT_PLANE::X_PLANE;

	glm::vec3 median_point= glm::vec3(0.f);
	float radius=0.f;	//for bounding sphere
	glm::vec3 min_point = glm::vec3(0.f);
	glm::vec3 max_point = glm::vec3(0.f);

	std::vector<glm::vec3> containing_vertices;

	unsigned int height = 1;

	Mesh sphere_mesh;
	Mesh aabb_mesh;

	TopDownNode* left = nullptr;
	TopDownNode* right = nullptr;
};