/* Start Header -------------------------------------------------------
Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BoundingVolume.cpp
Purpose: Implementation of BoundingVolume class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS350_2
Author: Seongwook,Shin    s.shin  180003319
Creation date: 03/08/2020
End Header --------------------------------------------------------*/ 
#include "BoundingVolume.h"
#include <iostream>
#include <cmath>

void CovarianceMatrix(glm::mat3x3& cov, std::vector<glm::vec3> pt, int numPts)
{
	glm::vec3 c = glm::vec3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < numPts; i++)
	{
		c += pt[i];
	}
	c /= numPts;

	for (int i = 0; i < numPts; i++)
	{
		pt[i] -= c;
	}

	for (int c = 0; c < 3; ++c)
	{
		for (int r = c; r < 3; ++r)
		{
			float& acc = cov[c][r];
			acc = 0.0f;

			// cov(X, Y) = E[(X - x)(Y - y)]
			for (int i = 0; i < numPts; ++i)
			{
				acc += pt[i][c] * pt[i][r];
			}

			acc /= static_cast<float>(numPts);

			cov[r][c] = acc;	// covariance matrix is symmetric
		}
	}

}

void SymSchur2(glm::mat3x3& a, int p, int q, float& c, float& s)
{
	if (abs(a[p][q]) > 0.0001f) {
		float r = (a[q][q] - a[p][p]) / (2.0f * a[p][q]);
		float t;
		if (r >= 0.0f)
			t = 1.0f / (r + sqrt(1.0f + r * r));
		else
			t = -1.0f / (-r + sqrt(1.0f + r * r));
		c = 1.0f / sqrt(1.0f + t * t);
		s = t * c;
	}
	else {
		c = 1.0f;
		s = 0.0f;
	}
}

void JacobiSolver(glm::mat3x3 covariance, float eValues[3], glm::vec3 eVec[3])
{
	double max;
	int row, col, i, j, itCnt = 0;
	double theta, sinTheta, cosTheta, sin2Theta, cos2Theta, covRow, covCol, covRC, covRI, covCI, eVecRI, eVecCI;
	while (1)
	{
		max = fabs(covariance[0][1]);
		row = 0;
		col = 1;
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				if (i != j && fabs(covariance[i][j]) > max)
				{
					max = fabs(covariance[i][j]);
					row = i;
					col = j;
				}
			}
		}
		if (itCnt > 30)
		{
			break;
		}
		//jacobi
		if (covariance[row][row] == covariance[col][col])
		{
			theta = 3.14 / 4;
		}
		else
		{
			theta = atan(2 * covariance[row][col] / (covariance[row][row] - covariance[col][col])) / 2;
		}
		covRow = covariance[row][row];
		covCol = covariance[col][col];
		covRC = covariance[row][col];
		sinTheta = sin(theta);
		cosTheta = cos(theta);
		sin2Theta = sin(2 * theta);
		cos2Theta = cos(2 * theta);
		covariance[row][row] = covRow * cosTheta * cosTheta + covCol * sinTheta * sinTheta + covRC * sin2Theta;
		covariance[col][col] = covRow * sinTheta * sinTheta + covCol * cosTheta * cosTheta - covRC * sin2Theta;
		covariance[row][col] = covariance[col][row] = (covCol - covRow) * sin2Theta / 2 + covRC * cos2Theta;

		for (i = 0; i < 3; i++)
		{
			if (i != row && i != col)
			{
				covRI = covariance[row][i];
				covCI = covariance[col][i];
				covariance[row][i] = covariance[i][row] = covRI * cosTheta + covCI * sinTheta;
				covariance[col][i] = covariance[i][col] = covCI * cosTheta - covRI * sinTheta;
			}
		}
		if (itCnt == 0)
		{
			eVec[row][row] = cosTheta;
			eVec[col][row] = -sinTheta;
			eVec[row][col] = sinTheta;
			eVec[col][col] = cosTheta;
		}
		else {
			for (i = 0; i < 3; i++)
			{
				eVecRI = eVec[row][i];
				eVecCI = eVec[col][i];
				eVec[row][i] = eVecRI * cosTheta + eVecCI * sinTheta;
				eVec[col][i] = eVecCI * cosTheta - eVecRI * sinTheta;
			}
		}
		itCnt++;
	}
	for (i = 0; i < 3; i++)
	{
		eValues[i] = covariance[i][i];
	}
}

void ExtremePointsAlongDirection(glm::vec3 dir, std::vector<glm::vec3>pt, int n, int* imin, int* imax)
{
	float minproj = FLT_MAX, maxproj = -FLT_MAX;
	for (int i = 0; i < n; i++) 
	{
		float proj = glm::dot(pt[i], dir);
		if (proj < minproj) 
		{
			minproj = proj;
			*imin = i;
		}
		if (proj > maxproj) 
		{
			maxproj = proj;
			*imax = i;
		}
	}
}

void BoundingVolume::ComputeBoundingVolume()
{
	CovarianceMatrix(cov_matrix, containing_vertices, containing_vertices.size());
	JacobiSolver(cov_matrix, eValue, eVector);
	Compute_PCA();
	owner->rotate_matrix = rot_matrix;

	ComputeAABB();
	Compute_centroid_sphere();
	Compute_ritter_sphere();
	Compute_PCA_sphere();
	Compute_Larsson_sphere();
	Compute_PCA_ellipsoid();
	Compute_OBB_PCA();
}

void BoundingVolume::ComputeAABB()
{
	min_point = containing_vertices[0];
	max_point = containing_vertices[0];

	for (auto& vert_iter : containing_vertices)
	{
		if (min_point.x > vert_iter.x)
		{
			min_point.x = vert_iter.x;
		}
		else if (max_point.x < vert_iter.x)
		{
			max_point.x = vert_iter.x;
		}

		if (min_point.y > vert_iter.y)
		{
			min_point.y = vert_iter.y;
		}
		else if (max_point.y < vert_iter.y)
		{
			max_point.y = vert_iter.y;
		}

		if (min_point.z > vert_iter.z)
		{
			min_point.z = vert_iter.z;
		}
		else if (max_point.z < vert_iter.z)
		{
			max_point.z = vert_iter.z;
		}
	}

	AABB.MakeAABB(min_point, max_point);
}

void BoundingVolume::Compute_centroid_sphere()
{
	float max_radius_squre = 0.f;
	float radius_square = 0.f;

	//find the farest vertex
	for (auto& vert_iter : containing_vertices)
	{
		radius_square = glm::dot(median_point - vert_iter, median_point - vert_iter);
		if (max_radius_squre < radius_square)
		{
			max_radius_squre = radius_square;
		}
	}
	radius_centroid = sqrt(max_radius_squre);

	Centroid_Sphere.SetScale(glm::vec3(radius_centroid));
	Centroid_Sphere.SetTranslate(median_point);
}

void BoundingVolume::Compute_ritter_sphere()
{
	int minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;

	for (int i = 1; i < containing_vertices.size(); i++)
	{
		if (containing_vertices[i].x < containing_vertices[minx].x) minx = i;
		if (containing_vertices[i].x > containing_vertices[maxx].x) maxx = i;
		if (containing_vertices[i].y < containing_vertices[miny].y) miny = i;
		if (containing_vertices[i].y > containing_vertices[maxy].y) maxy = i;
		if (containing_vertices[i].z < containing_vertices[minz].z) minz = i;
		if (containing_vertices[i].z > containing_vertices[maxz].z) maxz = i;
	}
	float distance_x_square = glm::dot(containing_vertices[maxx] - containing_vertices[minx], containing_vertices[maxx] - containing_vertices[minx]);
	float distance_y_square = glm::dot(containing_vertices[maxy] - containing_vertices[miny], containing_vertices[maxy] - containing_vertices[miny]);
	float distance_z_square = glm::dot(containing_vertices[maxz] - containing_vertices[minz], containing_vertices[maxz] - containing_vertices[minz]);

	//index for the longest axis
	int min_index = 0, max_index = 0;

	if (distance_x_square > distance_y_square&& distance_x_square > distance_z_square)
	{
		min_index = minx;
		max_index = maxx;
	}
	else if (distance_y_square > distance_x_square&& distance_y_square > distance_z_square)
	{
		min_index = miny;
		max_index = maxy;
	}
	else
	{
		min_index = minz;
		max_index = maxz;
	}

	center_ritter = (containing_vertices[min_index] + containing_vertices[max_index]) * 0.5f;
	radius_ritter = sqrt(glm::dot(containing_vertices[max_index] - center_ritter, containing_vertices[max_index] - center_ritter));

	for (auto& vert_iter : containing_vertices)
	{
		glm::vec3 d = vert_iter - center_ritter;
		float distance_square = glm::dot(d, d);

		if (distance_square > radius_ritter* radius_ritter)
		{
			float distance = sqrt(distance_square);
			float new_radius = (radius_ritter + distance) * 0.5f;
			float k = (new_radius - radius_ritter) / distance;
			radius_ritter = new_radius;
			center_ritter += d * k;
		}
	}

	Ritter_Sphere.SetScale(glm::vec3(radius_ritter));
	Ritter_Sphere.SetTranslate(center_ritter);
}

void BoundingVolume::Compute_PCA_sphere()
{
	glm::vec3 e;
	int maxc = 0;
	float maxf, maxe = std::abs(cov_matrix[0][0]);
	if ((maxf = std::abs(cov_matrix[1][1])) > maxe)
	{
		maxc = 1;
		maxe = maxf;
	}
	if ((maxf = std::abs(cov_matrix[2][2])) > maxe)
	{
		maxc = 2;
		maxe = maxf;
	}

	e[0] = eVector[0][maxc];
	e[1] = eVector[1][maxc];
	e[2] = eVector[2][maxc];

	int imin = 0, imax = 0;

	ExtremePointsAlongDirection(e, containing_vertices, containing_vertices.size(), &imin, &imax);

	glm::vec3 minpt = containing_vertices[imin];
	glm::vec3 maxpt = containing_vertices[imax];

	float dist = glm::length(maxpt - minpt);
	radius_PCA = dist * 0.5f;
	center_PCA_Sphere = (minpt + maxpt) * 0.5f;

	glm::vec3 d;
	float dist2 = 0.f;
	for (auto& vert_iter : containing_vertices)
	{
		d = vert_iter - center_PCA_Sphere;
		dist2 = dot(d, d);
		if (dist2 > radius_PCA* radius_PCA)
		{
			float dist = sqrt(dist2);
			float new_radius = (radius_PCA + dist) * 0.5f;
			float k = (new_radius - radius_PCA) / dist;
			radius_PCA = new_radius;
			center_PCA_Sphere += d * k;
		}
	}
	PCA_Sphere.SetScale(glm::vec3(radius_PCA));
	PCA_Sphere.SetTranslate(center_PCA_Sphere);
}

void BoundingVolume::Compute_Larsson_sphere()
{
	glm::vec3 e[3];
	e[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	e[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	e[2] = glm::vec3(0.0f, 0.0f, 1.0f);

	int imin, imax;
	ExtremePointsAlongDirection(e[0], containing_vertices, containing_vertices.size(), &imin, &imax);
	glm::vec3 minpt = containing_vertices[imin];
	glm::vec3 maxpt = containing_vertices[imax];
	radius_Larsson = glm::distance(maxpt, center_Larsson_Sphere);
	center_Larsson_Sphere = (minpt + maxpt) / 2.f;

	ExtremePointsAlongDirection(e[1], containing_vertices, containing_vertices.size(), &imin, &imax);
	minpt = containing_vertices[imin];
	maxpt = containing_vertices[imax]; 
	float dist2 = glm::distance(maxpt, center_Larsson_Sphere);
	if (dist2 < radius_Larsson) 
	{
		radius_Larsson = dist2;
		center_Larsson_Sphere = (minpt + maxpt) / 2.f;
	}

	ExtremePointsAlongDirection(e[2], containing_vertices, containing_vertices.size(), &imin, &imax);
	minpt = containing_vertices[imin];
	maxpt = containing_vertices[imax];
	float dist3 = glm::distance(maxpt, center_Larsson_Sphere);
	if (dist3 < radius_Larsson)
	{
		radius_Larsson = dist3;
		center_Larsson_Sphere = (minpt + maxpt) / 2.f;
	}

	Larsson_Sphere.SetScale(glm::vec3(radius_Larsson));
	Larsson_Sphere.SetTranslate(center_Larsson_Sphere);
}

void BoundingVolume::Compute_PCA_ellipsoid()
{
	center_Ellipsoid = median_point;
	center_Ellipsoid += (offset.x * result_orient[0]) + (offset.y * result_orient[1]) + (offset.z * result_orient[2]);

	radius_Ellipsoid = (maxExtent - minExtent) / 2.0f;

	PCA_Ellipsoid.SetScale(radius_Ellipsoid);
	PCA_Ellipsoid.rotate_matrix = rot_matrix;
	PCA_Ellipsoid.SetTranslate(center_Ellipsoid);
}

void BoundingVolume::Compute_OBB_PCA()
{
	center_OBB = median_point;
	center_OBB += (offset.x * result_orient[0]) +(offset.y * result_orient[1]) +(offset.z * result_orient[2]);

	extend_OBB = (maxExtent - minExtent) / 2.0f;


	PCA_OBB.MakeAABB(glm::vec3(-1.f, -1.f, -1.f), glm::vec3(1.f, 1.f, 1.f));
	PCA_OBB.rotate_matrix = rot_matrix;
	PCA_OBB.SetScale(extend_OBB);
	PCA_OBB.SetTranslate(center_OBB);
}

void BoundingVolume::Compute_PCA()
{
	for (auto& vert_iter : containing_vertices)
	{
		median_point += vert_iter;
	}
	median_point /= containing_vertices.size();

	float temp;
	glm::vec3 tempVec;

	if (eValue[0] <= eValue[1])
	{
		if (eValue[1] > eValue[2])
		{
			if (eValue[0] < eValue[2])
			{
				temp = eValue[0];
				tempVec = eVector[0];

				eValue[0] = eValue[2];
				eValue[2] = eValue[1];
				eValue[1] = temp;

				eVector[0] = eVector[2];
				eVector[2] = eVector[1];
				eVector[1] = tempVec;
			}
			else
			{
				temp = eValue[1];
				tempVec = eVector[1];

				eValue[1] = eValue[2];
				eValue[2] = temp;

				eVector[1] = eVector[2];
				eVector[2] = tempVec;
			}
		}
		// else do nothing
	}
	else
	{
		if (eValue[0] > eValue[2])
		{
			if (eValue[1] < eValue[2])
			{
				temp = eValue[0];
				tempVec = eVector[0];

				eValue[0] = eValue[1];
				eValue[1] = eValue[2];
				eValue[2] = temp;

				eVector[0] = eVector[1];
				eVector[1] = eVector[2];
				eVector[2] = tempVec;
			}
			else
			{
				temp = eValue[0];
				tempVec = eVector[0];

				eValue[0] = eValue[2];
				eValue[2] = temp;

				eVector[0] = eVector[2];
				eVector[2] = tempVec;
			}
		}
		else
		{
			temp = eValue[0];
			tempVec = eVector[0];

			eValue[0] = eValue[1];
			eValue[1] = temp;

			eVector[0] = eVector[1];
			eVector[1] = tempVec;
		}
	}

	result_orient[0] = eVector[2];
	result_orient[1] = eVector[1];
	result_orient[2] = eVector[0];

	result_orient[0] = glm::normalize(result_orient[0]);
	result_orient[1] -= (result_orient[1] * result_orient[0]) * result_orient[0];
	result_orient[1] = glm::normalize(result_orient[1]);
	result_orient[2] = glm::cross(result_orient[0], result_orient[1]);


	for (int index = 0; index < containing_vertices.size(); ++index)
	{
		glm::vec3 vec = containing_vertices[index];
		glm::vec3 displacement = vec - median_point;

		minExtent.x = fmin(minExtent.x, glm::dot(displacement, result_orient[0]));
		minExtent.y = fmin(minExtent.y, glm::dot(displacement, result_orient[1]));
		minExtent.z = fmin(minExtent.z, glm::dot(displacement, result_orient[2]));
		maxExtent.x = fmax(maxExtent.x, glm::dot(displacement, result_orient[0]));
		maxExtent.y = fmax(maxExtent.y, glm::dot(displacement, result_orient[1]));
		maxExtent.z = fmax(maxExtent.z, glm::dot(displacement, result_orient[2]));
	}

	offset = (maxExtent - minExtent) / 2.0f + minExtent;

	rot_matrix = glm::mat3x3(result_orient[0], result_orient[1], result_orient[2]);
}

void BoundingVolume::Draw_AABB()
{
	global_graphic->Draw_AABB(AABB);
}

void BoundingVolume::Draw_Centroid_Sphere()
{
	global_graphic->Draw_Face(Centroid_Sphere, SHADER_PROGRAM_TYPE::STANDARD_SHADER);
}

void BoundingVolume::Draw_PCA_Sphere()
{
	global_graphic->Draw_Face(PCA_Sphere, SHADER_PROGRAM_TYPE::STANDARD_SHADER);
}

void BoundingVolume::Draw_Ritter_Sphere()
{
	global_graphic->Draw_Face(Ritter_Sphere, SHADER_PROGRAM_TYPE::STANDARD_SHADER);
}

void BoundingVolume::Draw_Larsson_Sphere()
{
	global_graphic->Draw_Face(Larsson_Sphere, SHADER_PROGRAM_TYPE::STANDARD_SHADER);
}

void BoundingVolume::Draw_OBB_PCA()
{
	global_graphic->Draw_OBB(PCA_OBB);
}

void BoundingVolume::Draw_PCA_ellipsoid()
{
	global_graphic->Draw_Sphere(PCA_Ellipsoid);
}
