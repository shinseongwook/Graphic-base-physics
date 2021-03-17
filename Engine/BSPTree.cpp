#include "BSPTree.h"

void BSPTree::Compute_BSPTree()
{
	head_node->Compute_BSPTreeNode();
}

void BSPTree::Draw_BSPTree(int height)
{
	head_node->Draw_BSPTree(0);
}

void BSPTreeNode::Compute_BSPTreeNode()
{
	//if numbers of containing triangle is smaller than 300
	if (containing_vertices.size() / 3 < 300)
	{
		triangles_mesh.Vertices = containing_vertices;
		triangles_mesh.face_index.resize(containing_vertices.size());
		for (int i = 0; i < containing_vertices.size(); ++i)
		{
			triangles_mesh.face_index[i] = i;
		}

		return;
	}

	//get median (not good because of using indices)
	for (auto& iter : containing_vertices)
	{
		if (iter.x < min_point.x)
			min_point.x = iter.x;
		if (iter.y < min_point.y)
			min_point.y = iter.y;
		if (iter.z < min_point.z)
			min_point.z = iter.z;
		if (iter.x > max_point.x)
			max_point.x = iter.x;
		if (iter.y > max_point.y)
			max_point.y = iter.y;
		if (iter.z > max_point.z)
			max_point.z = iter.z;

		median_point += iter;
	}
	if (abs(max_point.x - min_point.x) < 0.01f || abs(max_point.y - min_point.y) < 0.01f || abs(max_point.z - min_point.z) < 0.01f)
	{
		triangles_mesh.Vertices = containing_vertices;
		triangles_mesh.face_index.resize(containing_vertices.size());
		for (int i = 0; i < containing_vertices.size(); ++i)
		{
			triangles_mesh.face_index[i] = i;
		}

		return;
	}

	median_point /= containing_vertices.size();

	//x?
	float x_judge_num = abs(max_point.x - min_point.x);
	float y_judge_num = abs(max_point.y - min_point.y);
	float z_judge_num = abs(max_point.z - min_point.z);

	//for (auto& iter : containing_vertices)
	//{
	//	iter.x > median_point.x ? ++x_judge_num : --x_judge_num;
	//	iter.y > median_point.y ? ++y_judge_num : --y_judge_num;
	//	iter.z > median_point.z ? ++z_judge_num : --z_judge_num;
	//}

	x_judge_num = abs(x_judge_num);
	y_judge_num = abs(y_judge_num);
	z_judge_num = abs(z_judge_num);

	front = new BSPTreeNode;
	back = new BSPTreeNode;

	//is front==1 back=0
	int triangle_front[3] = { 0 };
	int front_num = 0;


	//if x plane is split plane
	if (x_judge_num >= y_judge_num && x_judge_num >= z_judge_num)
	{
		for (int i = 0; i < containing_vertices.size(); i += 3)
		{
			front_num = 0;

			triangle_front[0] = containing_vertices[i].x > median_point.x ? ++front_num, 1 : 0;
			triangle_front[1] = containing_vertices[i + 1].x > median_point.x ? ++front_num, 1 : 0;
			triangle_front[2] = containing_vertices[i + 2].x > median_point.x ? ++front_num, 1 : 0;

			//is all same?
			if (triangle_front[0] == triangle_front[1] && triangle_front[0] == triangle_front[2])
			{
				if (triangle_front[0] == 1)
				{
					front->containing_vertices.push_back(containing_vertices[i]);
					front->containing_vertices.push_back(containing_vertices[i + 1]);
					front->containing_vertices.push_back(containing_vertices[i + 2]);
				}
				else
				{
					back->containing_vertices.push_back(containing_vertices[i]);
					back->containing_vertices.push_back(containing_vertices[i + 1]);
					back->containing_vertices.push_back(containing_vertices[i + 2]);
				}
			}
			else // 2, 1
			{
				//select solo one
				glm::vec3 alone_vec = containing_vertices[i];
				glm::vec3 one_vec = containing_vertices[i + 1];
				glm::vec3 two_vec = containing_vertices[i + 2];

				glm::vec3 one_intersect_vec;
				glm::vec3 two_intersect_vec;

				if (front_num == 1)
				{
					if (alone_vec.x <= median_point.x)
					{
						if (one_vec.x > median_point.x)
						{
							std::swap(alone_vec, one_vec);
						}
						else
						{
							std::swap(alone_vec, two_vec);
						}
					}
					one_intersect_vec = Get_Intersect_Point(alone_vec, one_vec, 0);
					two_intersect_vec = Get_Intersect_Point(alone_vec, two_vec, 0);

					front->containing_vertices.push_back(alone_vec);
					front->containing_vertices.push_back(one_intersect_vec);
					front->containing_vertices.push_back(two_intersect_vec);

					back->containing_vertices.push_back(one_vec);
					back->containing_vertices.push_back(one_intersect_vec);
					back->containing_vertices.push_back(two_vec);

					back->containing_vertices.push_back(two_vec);
					back->containing_vertices.push_back(one_intersect_vec);
					back->containing_vertices.push_back(two_intersect_vec);
				}
				else if (front_num == 2)
				{
					if (alone_vec.x >= median_point.x)
					{
						if (one_vec.x < median_point.x)
						{
							std::swap(alone_vec, one_vec);
						}
						else
						{
							std::swap(alone_vec, two_vec);
						}
					}
					one_intersect_vec = Get_Intersect_Point(alone_vec, one_vec, 0);
					two_intersect_vec = Get_Intersect_Point(alone_vec, two_vec, 0);

					back->containing_vertices.push_back(alone_vec);
					back->containing_vertices.push_back(one_intersect_vec);
					back->containing_vertices.push_back(two_intersect_vec);

					front->containing_vertices.push_back(one_vec);
					front->containing_vertices.push_back(one_intersect_vec);
					front->containing_vertices.push_back(two_vec);

					front->containing_vertices.push_back(two_vec);
					front->containing_vertices.push_back(one_intersect_vec);
					front->containing_vertices.push_back(two_intersect_vec);
				}
			}
		}
	}
	//if y plane is split plane
	else if (y_judge_num >= x_judge_num && y_judge_num >= z_judge_num)
	{
		for (int i = 0; i < containing_vertices.size(); i += 3)
		{
			front_num = 0;

			triangle_front[0] = containing_vertices[i].y > median_point.y ? ++front_num, 1 : 0;
			triangle_front[1] = containing_vertices[i + 1].y > median_point.y ? ++front_num, 1 : 0;
			triangle_front[2] = containing_vertices[i + 2].y > median_point.y ? ++front_num, 1 : 0;

			//is all same?
			if (triangle_front[0] == triangle_front[1] && triangle_front[0] == triangle_front[2])
			{
				if (triangle_front[0] == 1)
				{
					front->containing_vertices.push_back(containing_vertices[i]);
					front->containing_vertices.push_back(containing_vertices[i + 1]);
					front->containing_vertices.push_back(containing_vertices[i + 2]);
				}
				else
				{
					back->containing_vertices.push_back(containing_vertices[i]);
					back->containing_vertices.push_back(containing_vertices[i + 1]);
					back->containing_vertices.push_back(containing_vertices[i + 2]);
				}
			}
			else // 2, 1
			{
				//select solo one
				glm::vec3 alone_vec = containing_vertices[i];
				glm::vec3 one_vec = containing_vertices[i + 1];
				glm::vec3 two_vec = containing_vertices[i + 2];

				glm::vec3 one_intersect_vec;
				glm::vec3 two_intersect_vec;

				if (front_num == 1)
				{
					if (alone_vec.y <= median_point.y)
					{
						if (one_vec.y > median_point.y)
						{
							std::swap(alone_vec, one_vec);
						}
						else
						{
							std::swap(alone_vec, two_vec);
						}
					}
					one_intersect_vec = Get_Intersect_Point(alone_vec, one_vec, 1);
					two_intersect_vec = Get_Intersect_Point(alone_vec, two_vec, 1);

					front->containing_vertices.push_back(alone_vec);
					front->containing_vertices.push_back(one_intersect_vec);
					front->containing_vertices.push_back(two_intersect_vec);

					back->containing_vertices.push_back(one_vec);
					back->containing_vertices.push_back(one_intersect_vec);
					back->containing_vertices.push_back(two_vec);

					back->containing_vertices.push_back(two_vec);
					back->containing_vertices.push_back(one_intersect_vec);
					back->containing_vertices.push_back(two_intersect_vec);
				}
				else if (front_num == 2)
				{
					if (alone_vec.y >= median_point.y)
					{
						if (one_vec.y < median_point.y)
						{
							std::swap(alone_vec, one_vec);
						}
						else
						{
							std::swap(alone_vec, two_vec);
						}
					}
					one_intersect_vec = Get_Intersect_Point(alone_vec, one_vec, 1);
					two_intersect_vec = Get_Intersect_Point(alone_vec, two_vec, 1);

					back->containing_vertices.push_back(alone_vec);
					back->containing_vertices.push_back(one_intersect_vec);
					back->containing_vertices.push_back(two_intersect_vec);

					front->containing_vertices.push_back(one_vec);
					front->containing_vertices.push_back(one_intersect_vec);
					front->containing_vertices.push_back(two_vec);

					front->containing_vertices.push_back(two_vec);
					front->containing_vertices.push_back(one_intersect_vec);
					front->containing_vertices.push_back(two_intersect_vec);
				}
			}
		}
	}
	//if z plane is split plane
	else if (z_judge_num >= x_judge_num && z_judge_num >= y_judge_num)
	{
		for (int i = 0; i < containing_vertices.size(); i += 3)
		{
			front_num = 0;

			triangle_front[0] = containing_vertices[i].z > median_point.z ? ++front_num, 1 : 0;
			triangle_front[1] = containing_vertices[i + 1].z > median_point.z ? ++front_num, 1 : 0;
			triangle_front[2] = containing_vertices[i + 2].z > median_point.z ? ++front_num, 1 : 0;

			//is all same?
			if (triangle_front[0] == triangle_front[1] && triangle_front[0] == triangle_front[2])
			{
				if (triangle_front[0] == 1)
				{
					front->containing_vertices.push_back(containing_vertices[i]);
					front->containing_vertices.push_back(containing_vertices[i + 1]);
					front->containing_vertices.push_back(containing_vertices[i + 2]);
				}
				else
				{
					back->containing_vertices.push_back(containing_vertices[i]);
					back->containing_vertices.push_back(containing_vertices[i + 1]);
					back->containing_vertices.push_back(containing_vertices[i + 2]);
				}
			}
			else // 2, 1
			{
				//select solo one
				glm::vec3 alone_vec = containing_vertices[i];
				glm::vec3 one_vec = containing_vertices[i + 1];
				glm::vec3 two_vec = containing_vertices[i + 2];

				glm::vec3 one_intersect_vec;
				glm::vec3 two_intersect_vec;

				if (front_num == 1)
				{
					if (alone_vec.z <= median_point.z)
					{
						if (one_vec.z > median_point.z)
						{
							std::swap(alone_vec, one_vec);
						}
						else
						{
							std::swap(alone_vec, two_vec);
						}
					}
					one_intersect_vec = Get_Intersect_Point(alone_vec, one_vec, 2);
					two_intersect_vec = Get_Intersect_Point(alone_vec, two_vec, 2);

					front->containing_vertices.push_back(alone_vec);
					front->containing_vertices.push_back(one_intersect_vec);
					front->containing_vertices.push_back(two_intersect_vec);

					back->containing_vertices.push_back(one_vec);
					back->containing_vertices.push_back(one_intersect_vec);
					back->containing_vertices.push_back(two_vec);

					back->containing_vertices.push_back(two_vec);
					back->containing_vertices.push_back(one_intersect_vec);
					back->containing_vertices.push_back(two_intersect_vec);
				}
				else if (front_num == 2)
				{
					if (alone_vec.z >= median_point.z)
					{
						if (one_vec.z < median_point.z)
						{
							std::swap(alone_vec, one_vec);
						}
						else
						{
							std::swap(alone_vec, two_vec);
						}
					}
					one_intersect_vec = Get_Intersect_Point(alone_vec, one_vec, 2);
					two_intersect_vec = Get_Intersect_Point(alone_vec, two_vec, 2);

					back->containing_vertices.push_back(alone_vec);
					back->containing_vertices.push_back(one_intersect_vec);
					back->containing_vertices.push_back(two_intersect_vec);

					front->containing_vertices.push_back(one_vec);
					front->containing_vertices.push_back(one_intersect_vec);
					front->containing_vertices.push_back(two_vec);

					front->containing_vertices.push_back(two_vec);
					front->containing_vertices.push_back(one_intersect_vec);
					front->containing_vertices.push_back(two_intersect_vec);
				}
			}
		}
	}
	containing_vertices.clear();

	if (front->containing_vertices.size() == 0)
	{
		delete front;
		front = nullptr;
	}
	if (back->containing_vertices.size() == 0)
	{
		delete back;
		back = nullptr;
	}
	if (front)
	{
		front->Compute_BSPTreeNode();
	}
	if (back)
	{
		back->Compute_BSPTreeNode();
	}
	return;
}


//p1 is alone
glm::vec3 BSPTreeNode::Get_Intersect_Point(glm::vec3& p1, glm::vec3& p2, int plane)
{
	glm::vec3 direc_vec = (p1 - p2);

	glm::vec3 result;

	float ratio = 0.f;
	if (plane == 0)
	{
		if (p1.x == p2.x)
		{
			return p1;
		}
		ratio = abs(median_point.x - p2.x) / abs(p1.x - p2.x);
	}
	else if (plane == 1)
	{
		if (p1.y == p2.y)
		{
			return p1;
		}
		ratio = abs(median_point.y - p2.y) / abs(p1.y - p2.y);
	}
	else
	{
		if (p1.z == p2.z)
		{
			return p1;
		}
		ratio = abs(median_point.z - p2.z) / abs(p1.z - p2.z);
	}

	result = p2 + direc_vec * ratio;

	if (result.x > 1.f)
		int a = 1;
	if (result.y > 1.f)
		int a = 1;
	if (result.z > 1.f)
		int a = 1;

	return result;
}

void BSPTreeNode::Draw_BSPTree(int height)
{
	if (front)
	{
		front->Draw_BSPTree(height + 1);
	}
	if (back)
	{
		back->Draw_BSPTree(height + 1);
	}
	global_graphic->Draw_Triangles_line_Only(triangles_mesh, height);

}
