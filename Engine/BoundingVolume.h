/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BoundingVolume.h
Purpose: Definition of BoundingVolume class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS350_2
Author: Seongwook,Shin    s.shin  180003319
Creation date: 03/08/2020
End Header --------------------------------------------------------*/ 
#pragma once
#include "Graphic.h"
#include "Mesh.h"

constexpr float infinity = std::numeric_limits<float>::infinity();

struct BoundingVolume
{
	BoundingVolume()
	{
		Centroid_Sphere.MakeSphere(1.f, 20);
		Ritter_Sphere.MakeSphere(1.f, 20);
		PCA_Sphere.MakeSphere(1.f, 20);
		Larsson_Sphere.MakeSphere(1.f, 20);
		PCA_Ellipsoid.MakeSphere(1.f, 20);
		for (int i = 0; i < 3; ++i)
		{
			eVector[i] = glm::vec3(0.f);
			result_orient[i] = glm::vec3(0.f);
		}
		offset= glm::vec3(0.f);
	}
	void ComputeBoundingVolume();
	
	void ComputeAABB();
	void Compute_centroid_sphere();
	void Compute_ritter_sphere();
	void Compute_PCA_sphere();
	void Compute_Larsson_sphere();
	void Compute_PCA_ellipsoid();
	void Compute_OBB_PCA();

	void Compute_PCA();


	void Draw_AABB();
	void Draw_Centroid_Sphere();
	void Draw_PCA_Sphere();
	void Draw_Ritter_Sphere();
	void Draw_Larsson_Sphere();
	void Draw_OBB_PCA();
	void Draw_PCA_ellipsoid();

	glm::mat3x3 cov_matrix = glm::mat3x3(0);
	float eValue[3] = {0.f};
	glm::vec3 eVector[3];
	glm::vec3 result_orient[3];
	glm::vec3 minExtent = glm::vec3(infinity, infinity, infinity);
	glm::vec3 maxExtent = glm::vec3(-infinity,-infinity,-infinity);
	glm::vec3 offset;
	glm::mat3x3 rot_matrix = glm::mat3x3(0);

	std::vector<glm::vec3> containing_vertices;
	glm::vec3 median_point = glm::vec3(0.f);
	float radius_centroid = 0.f;
	glm::vec3 center_ritter = glm::vec3(0.f);
	float radius_ritter = 0.f;
	glm::vec3 center_PCA_Sphere = glm::vec3(0.f);
	float radius_PCA = 0.f;
	glm::vec3 center_Larsson_Sphere = glm::vec3(0.f);
	float radius_Larsson = 0.f;



	glm::vec3 center_Ellipsoid = glm::vec3(0.f);
	glm::vec3 radius_Ellipsoid = glm::vec3(0.f);

	glm::vec3 center_OBB = glm::vec3(0.f);
	glm::vec3 extend_OBB = glm::vec3(0.f);

	glm::vec3 min_point = glm::vec3(0.f);
	glm::vec3 max_point = glm::vec3(0.f);


	Mesh AABB;
	Mesh Centroid_Sphere;
	Mesh Ritter_Sphere;
	Mesh PCA_Sphere;
	Mesh Larsson_Sphere;

	Mesh PCA_Ellipsoid;
	Mesh PCA_OBB;

	Mesh* owner = nullptr;
};