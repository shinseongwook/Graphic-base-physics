/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: TopDownTree.cpp
Purpose: Implementation of TopDownTree class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS350_2
Author: Seongwook,Shin    s.shin  180003319
Creation date: 03/08/2020
End Header --------------------------------------------------------*/ 
#include "TopDownTree.h"

void TopDownTree::Compute_AABB()
{
	head_node->Compute_HeadAABB();
	head_node->Compute_TopDownAABB();

	return;
}

void TopDownTree::Compute_BS()
{
	head_node->Compute_HeadBS();
	head_node->Compute_TopDownBS();

	return;
}

void TopDownTree::Draw_AABB(int height)
{
	head_node->Draw_AABB(height);
}

void TopDownTree::Draw_Sphere(int height)
{
	head_node->Draw_Bounding_Sphere(height);

	return;
}

void TopDownNode::Compute_HeadAABB()
{
	for (auto& iter : containing_vertices)
	{
		Check_MinMax(iter);
	}
	aabb_mesh.MakeAABB(min_point, max_point);
}

void TopDownNode::Compute_HeadBS()
{
	float distance = 0.f;
	glm::vec3 distance_vector;
	for (auto& iter : containing_vertices)
	{
		distance_vector = median_point - iter;
		distance = glm::dot(distance_vector, distance_vector);
		if (distance > radius)
		{
			radius = distance;
		}
	}
	radius = sqrt(radius);

	return;
}

void TopDownNode::Compute_TopDownAABB()
{
	if (containing_vertices.size() < 500||height==7)
	{
		return;
	}

	TopDownNode left_x_plane;
	TopDownNode left_y_plane;
	TopDownNode left_z_plane;
	TopDownNode right_x_plane;
	TopDownNode right_y_plane;
	TopDownNode right_z_plane;


	for (auto& iter : containing_vertices)
	{
		if (iter.x < median_point.x)
		{
			left_x_plane.containing_vertices.push_back(iter);
			left_x_plane.median_point += iter;
			left_x_plane.Check_MinMax(iter);
		}
		else
		{
			right_x_plane.containing_vertices.push_back(iter);
			right_x_plane.median_point += iter;
			right_x_plane.Check_MinMax(iter);
		}

		//y plane
		if (iter.y < median_point.y)
		{
			left_y_plane.containing_vertices.push_back(iter);
			left_y_plane.median_point += iter;
			left_y_plane.Check_MinMax(iter);
		}
		else
		{
			right_y_plane.containing_vertices.push_back(iter);
			right_y_plane.median_point += iter;
			right_y_plane.Check_MinMax(iter);
		}

		//z plane
		if (iter.z < median_point.z)
		{
			left_z_plane.containing_vertices.push_back(iter);
			left_z_plane.median_point += iter;
			left_z_plane.Check_MinMax(iter);
		}
		else
		{
			right_z_plane.containing_vertices.push_back(iter);
			right_z_plane.median_point += iter;
			right_z_plane.Check_MinMax(iter);
		}
	}
	float x_plane_AABB_volume = left_x_plane.Compute_AABB_Volume() + right_x_plane.Compute_AABB_Volume();
	float y_plane_AABB_volume = left_y_plane.Compute_AABB_Volume() + right_y_plane.Compute_AABB_Volume();
	float z_plane_AABB_volume = left_z_plane.Compute_AABB_Volume() + right_z_plane.Compute_AABB_Volume();

	if (x_plane_AABB_volume <= y_plane_AABB_volume && x_plane_AABB_volume <= z_plane_AABB_volume)
	{
		plane = SPLIT_PLANE::X_PLANE;
		left_x_plane.median_point /= left_x_plane.containing_vertices.size();
		right_x_plane.median_point /= right_x_plane.containing_vertices.size();

		left = new TopDownNode(left_x_plane);
		right = new TopDownNode(right_x_plane);
	}
	else if (y_plane_AABB_volume <= x_plane_AABB_volume && y_plane_AABB_volume <= z_plane_AABB_volume)
	{
		plane = SPLIT_PLANE::Y_PLANE;
		left_y_plane.median_point /= left_y_plane.containing_vertices.size();
		right_y_plane.median_point /= right_y_plane.containing_vertices.size();

		left = new TopDownNode(left_y_plane);
		right = new TopDownNode(right_y_plane);
	}
	else if (z_plane_AABB_volume <= y_plane_AABB_volume && z_plane_AABB_volume <= x_plane_AABB_volume)
	{
		plane = SPLIT_PLANE::Z_PLANE;
		left_z_plane.median_point /= left_z_plane.containing_vertices.size();
		right_z_plane.median_point /= right_z_plane.containing_vertices.size();

		left = new TopDownNode(left_z_plane);
		right = new TopDownNode(right_z_plane);
	}

	if (left != nullptr)
	{
		left->aabb_mesh.MakeAABB(left->min_point, left->max_point);
		left->height = height + 1;
		left->Compute_TopDownAABB();
	}
	if (right != nullptr)
	{
		right->aabb_mesh.MakeAABB(right->min_point, right->max_point);
		right->height = height + 1;
		right->Compute_TopDownAABB();
	}
}

void TopDownNode::Compute_TopDownBS()
{
	if (containing_vertices.size() < 500 || height == 7)
	{
		return;
	}

	TopDownNode left_x_plane;
	TopDownNode left_y_plane;
	TopDownNode left_z_plane;
	TopDownNode right_x_plane;
	TopDownNode right_y_plane;
	TopDownNode right_z_plane;


	for (auto& iter : containing_vertices)
	{
		if (iter.x < median_point.x)
		{
			left_x_plane.containing_vertices.push_back(iter);
			left_x_plane.median_point += iter;
		}
		else
		{
			right_x_plane.containing_vertices.push_back(iter);
			right_x_plane.median_point += iter;
		}

		//y plane
		if (iter.y < median_point.y)
		{
			left_y_plane.containing_vertices.push_back(iter);
			left_y_plane.median_point += iter;
		}
		else
		{
			right_y_plane.containing_vertices.push_back(iter);
			right_y_plane.median_point += iter;
		}

		//z plane
		if (iter.z < median_point.z)
		{
			left_z_plane.containing_vertices.push_back(iter);
			left_z_plane.median_point += iter;
		}
		else
		{
			right_z_plane.containing_vertices.push_back(iter);
			right_z_plane.median_point += iter;
		}
	}

	left_x_plane.Compute_Radius();
	left_y_plane.Compute_Radius();
	left_z_plane.Compute_Radius();
	right_x_plane.Compute_Radius();
	right_y_plane.Compute_Radius();
	right_z_plane.Compute_Radius();

	float x_plane_BS_volume = left_x_plane.Compute_BS_Volume() + right_x_plane.Compute_BS_Volume();
	float y_plane_BS_volume = left_y_plane.Compute_BS_Volume() + right_y_plane.Compute_BS_Volume();
	float z_plane_BS_volume = left_z_plane.Compute_BS_Volume() + right_z_plane.Compute_BS_Volume();

	if (x_plane_BS_volume <= y_plane_BS_volume && x_plane_BS_volume <= z_plane_BS_volume)
	{
		plane = SPLIT_PLANE::X_PLANE;

		left = new TopDownNode(left_x_plane);
		right = new TopDownNode(right_x_plane);
	}
	else if (y_plane_BS_volume <= x_plane_BS_volume && y_plane_BS_volume <= z_plane_BS_volume)
	{
		plane = SPLIT_PLANE::Y_PLANE;

		left = new TopDownNode(left_y_plane);
		right = new TopDownNode(right_y_plane);
	}
	else if (z_plane_BS_volume <= y_plane_BS_volume && z_plane_BS_volume <= x_plane_BS_volume)
	{
		plane = SPLIT_PLANE::Z_PLANE;

		left = new TopDownNode(left_z_plane);
		right = new TopDownNode(right_z_plane);
	}

	if (left != nullptr)
	{
		left->height = height + 1;
		left->Compute_TopDownBS();
	}
	if (right != nullptr)
	{
		right->height = height + 1;
		right->Compute_TopDownBS();
	}
}

void TopDownNode::Check_MinMax(glm::vec3 vertex)
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

void TopDownNode::Compute_Radius()
{
	median_point /= containing_vertices.size();
	float distance = 0.f;
	glm::vec3 distance_vector;
	for (auto& iter : containing_vertices)
	{
		distance_vector = median_point - iter;
		distance = glm::dot(distance_vector,distance_vector);
		if (distance > radius)
		{
			radius = distance;
		}
	}
	radius = sqrt(radius);

	return;
}

float TopDownNode::Compute_AABB_Volume()
{
	float x_lenght = max_point.x - min_point.x;
	float y_lenght = max_point.y - min_point.y;
	float z_lenght = max_point.z - min_point.z;

	return x_lenght*y_lenght*z_lenght;
}

float TopDownNode::Compute_BS_Volume()
{
	return radius*radius*radius;
}

void TopDownNode::Draw_AABB(int aim)
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
		global_graphic->Draw_AABB(aabb_mesh,height);
	}
}

void TopDownNode::Draw_Bounding_Sphere(int aim)
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
		global_graphic->Draw_Face(sphere_mesh,SHADER_PROGRAM_TYPE::STANDARD_SHADER, height);
	}
}
