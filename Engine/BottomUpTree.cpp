/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BottomUpTree.cpp
Purpose: Implementation of BottomUpTree class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS350_2
Author: Seongwook,Shin    s.shin  180003319
Creation date: 03/08/2020
End Header --------------------------------------------------------*/ 
#include "BottomUpTree.h"

void BottomUpTree::Compute_AABB()
{
	//first, get all nodes' median and min max
	for (auto& iter : node_vector)
	{
		for (auto& vert_iter : iter->containing_vertices)
		{
			iter->median_point += vert_iter;
			iter->Check_MinMax(vert_iter);
		}
		iter->median_point /= iter->containing_vertices.size();
		iter->aabb_mesh.MakeAABB(iter->min_point, iter->max_point);
	}

	while (node_vector.size() > 1)
	{
		int i = 0;
		int j = 0;

		Find_Nearest_Object_Indices_AABB(i, j);
		BottomUpNode* new_node = new BottomUpNode;
		new_node->left = node_vector[i];
		new_node->right = node_vector[j];
		for (auto& iter : node_vector[i]->containing_vertices)
		{
			new_node->containing_vertices.push_back(iter);
		}
		for (auto& iter : node_vector[j]->containing_vertices)
		{
			new_node->containing_vertices.push_back(iter);
		}

		new_node->Make_AABB();

		node_vector.erase(node_vector.begin() + j);
		node_vector.erase(node_vector.begin() + i);

		node_vector.push_back(new_node);
	}
	head_node = node_vector[0];
	longest_height=head_node->ComputeHeight();

	return;

}

void BottomUpTree::Compute_BS()
{
	//first, get all nodes' median
	for (auto& iter : node_vector)
	{
		for (auto& vert_iter : iter->containing_vertices)
		{
			iter->median_point += vert_iter;
		}
		iter->median_point /= iter->containing_vertices.size();
	}
	//second, get all nodes' radius
	for (auto& iter : node_vector)
	{
		glm::vec3 distance_vector;
		float distance = 0.f;
		for (auto& vert_iter : iter->containing_vertices)
		{
			distance_vector = iter->median_point - vert_iter;
			distance = glm::dot(distance_vector, distance_vector);
			if (distance > iter->radius)
			{
				iter->radius = distance;
			}
		}
		iter->radius = sqrt(iter->radius);
	}

	while (node_vector.size() > 1)
	{
		int i = 0;
		int j = 0;

		Find_Nearest_Object_Indices_BS(i, j);
		BottomUpNode* new_node=new BottomUpNode;
		new_node->left = node_vector[i];
		new_node->right = node_vector[j];
		for (auto& iter : node_vector[i]->containing_vertices)
		{
			new_node->containing_vertices.push_back(iter);
		}
		for (auto& iter : node_vector[j]->containing_vertices)
		{
			new_node->containing_vertices.push_back(iter);
		}

		new_node->Make_BS();

		node_vector.erase(node_vector.begin() + j);
		node_vector.erase(node_vector.begin() + i);

		node_vector.push_back(new_node);
	}
	head_node = node_vector[0];
	longest_height=head_node->ComputeHeight();

	return;
}

void BottomUpTree::Find_Nearest_Object_Indices_AABB(int& i, int& j)
{
	float minimum_distance = FLT_MAX;
	for (int x = 0; x < node_vector.size(); ++x)
	{
		float distance = 0.f;
		for (int y = x + 1; y < node_vector.size(); ++y)
		{
			distance = glm::distance(node_vector[x]->median_point, node_vector[y]->median_point);

			if (distance < minimum_distance)
			{
				minimum_distance = distance;
				i = x;
				j = y;
			}
		}
	}
}

void BottomUpTree::Find_Nearest_Object_Indices_BS(int& i, int& j)
{
	float minimum_distance = FLT_MAX;
	for (int x = 0; x < node_vector.size(); ++x)
	{
		float distance=0.f;
		for (int y = x+1; y < node_vector.size(); ++y)
		{
			distance = glm::distance(node_vector[x]->median_point, node_vector[y]->median_point)- node_vector[x]->radius- node_vector[y]->radius;
			
			if (distance < minimum_distance)
			{
				minimum_distance = distance;
				i = x;
				j = y;
			}
		}
	}
}

void BottomUpTree::Draw_AABB(int height)
{

	height = longest_height + 1 - height;

	head_node->Draw_AABB(height);

	return;
}

void BottomUpTree::Draw_Sphere(int height)
{
	height = longest_height + 1 - height;

	head_node->Draw_Bounding_Sphere(height);

	return;
}

void BottomUpNode::Make_AABB()
{
	for (auto& iter : containing_vertices)
	{
		median_point += iter;
		Check_MinMax(iter);
	}
	median_point /= containing_vertices.size();
	aabb_mesh.MakeAABB(min_point, max_point);
}

void BottomUpNode::Make_BS()
{
	//first, get all nodes' median

	for (auto& vert_iter : containing_vertices)
	{
		median_point += vert_iter;
	}
	median_point /= containing_vertices.size();

	//second, get all nodes' radius

	glm::vec3 distance_vector;
	float distance = 0.f;
	for (auto& vert_iter : containing_vertices)
	{
		distance_vector = median_point - vert_iter;
		distance = glm::dot(distance_vector, distance_vector);
		if (distance > radius)
		{
			radius = distance;
		}
	}
	radius = sqrt(radius);
}

void BottomUpNode::Check_MinMax(glm::vec3 vertex)
{
	if (min_point == glm::vec3(0.f))
	{
		min_point = vertex;
		max_point = vertex;
		return;
	}

	if (min_point.x > vertex.x)
	{
		min_point.x = vertex.x;
	}
	else if (max_point.x < vertex.x)
	{
		max_point.x = vertex.x;
	}

	if (min_point.y > vertex.y)
	{
		min_point.y = vertex.y;
	}
	else if (max_point.y < vertex.y)
	{
		max_point.y = vertex.y;
	}

	if (min_point.z > vertex.z)
	{
		min_point.z = vertex.z;
	}
	else if (max_point.z < vertex.z)
	{
		max_point.z = vertex.z;
	}
}

int BottomUpNode::ComputeHeight()
{
	int longest = height;

	if (left != nullptr)
	{
		left->height = height + 1;
		int left_longest = left->ComputeHeight();
		if (left_longest > longest)
		{
			longest = left_longest;
		}
	}
	if (right != nullptr)
	{
		right->height = height + 1;;
		int right_longest = right->ComputeHeight();
		if (right_longest > longest)
		{
			longest = right_longest;
		}
	}
	return longest;
}

void BottomUpNode::Draw_AABB(int aim)
{
	if (height < aim)
	{
		if (left != nullptr)
		{
			left->Draw_AABB(aim);
		}
		if (right != nullptr)
		{
			right->Draw_AABB(aim);
		}
	}
	else if (height == aim)
	{
		global_graphic->Draw_AABB(aabb_mesh, height);
	}
}

void BottomUpNode::Draw_Bounding_Sphere(int aim)
{
	if (height < aim)
	{
		if (left != nullptr)
		{
			left->Draw_Bounding_Sphere(aim);
		}
		if (right != nullptr)
		{
			right->Draw_Bounding_Sphere(aim);
		}
	}
	else if (height == aim)
	{
		sphere_mesh.SetTranslate(median_point);
		sphere_mesh.SetScale(glm::vec3(radius));
		global_graphic->Draw_Face(sphere_mesh, SHADER_PROGRAM_TYPE::STANDARD_SHADER,height);
	}
}
