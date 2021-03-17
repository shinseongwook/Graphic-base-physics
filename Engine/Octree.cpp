#include "Octree.h"

void Octree::Compute_Octree()
{
	head_node->Compute_OctreeNode();
}

void Octree::Draw_Octree(int height)
{
	head_node->Draw_Octree(height);
}

void OctreeNode::Compute_OctreeNode()
{
	aabb_mesh.MakeAABB(min_point, max_point);
	//if numbers of containing triangle is smaller than 300
	if (containing_vertices.size() / 3 < 1000)
	{
		triangles_mesh.Vertices = containing_vertices;
		
		triangles_mesh.face_index.resize(containing_vertices.size());

		for (int i = 0; i < containing_vertices.size(); ++i)
		{
			triangles_mesh.face_index[i] = i;
		}

		return;
	}

	//---,--+,-+-,-++,+--,+-+,++-,+++

	int quadrant[8] = { 0 };
	int triangle_quadrant[3] = { 0 };
	for (int i = 0; i < containing_vertices.size(); i+=3)
	{
		memset(quadrant, 0, sizeof(int)*8);

		triangle_quadrant[0] = Judge_quadrant(containing_vertices[i]);
		triangle_quadrant[1] = Judge_quadrant(containing_vertices[i + 1]);
		triangle_quadrant[2] = Judge_quadrant(containing_vertices[i + 2]);

		for (int j = 0; j < 3; ++j)
		{
			if (child[triangle_quadrant[j]] == nullptr)
			{
				child[triangle_quadrant[j]] = new OctreeNode;
				switch (triangle_quadrant[j])
				{
				case 0:
					child[triangle_quadrant[j]]->center_point = (center_point + min_point) / 2.f;
					child[triangle_quadrant[j]]->min_point = min_point;
					child[triangle_quadrant[j]]->max_point = center_point;
					break;
				case 1:
					child[triangle_quadrant[j]]->center_point.x = (center_point.x + min_point.x) / 2.f;
					child[triangle_quadrant[j]]->center_point.y = (center_point.y + min_point.y) / 2.f;
					child[triangle_quadrant[j]]->center_point.z = (center_point.z + max_point.z) / 2.f;

					child[triangle_quadrant[j]]->min_point.x = min_point.x;
					child[triangle_quadrant[j]]->min_point.y = min_point.y;
					child[triangle_quadrant[j]]->min_point.z = center_point.z;

					child[triangle_quadrant[j]]->max_point.x = center_point.x;
					child[triangle_quadrant[j]]->max_point.y = center_point.y;
					child[triangle_quadrant[j]]->max_point.z = max_point.z;
					break;
				case 2:
					child[triangle_quadrant[j]]->center_point.x = (center_point.x + min_point.x) / 2.f;
					child[triangle_quadrant[j]]->center_point.y = (center_point.y + max_point.y) / 2.f;
					child[triangle_quadrant[j]]->center_point.z = (center_point.z + min_point.z) / 2.f;

					child[triangle_quadrant[j]]->min_point.x = min_point.x;
					child[triangle_quadrant[j]]->min_point.y = center_point.y;
					child[triangle_quadrant[j]]->min_point.z = min_point.z;

					child[triangle_quadrant[j]]->max_point.x = center_point.x;
					child[triangle_quadrant[j]]->max_point.y = max_point.y;
					child[triangle_quadrant[j]]->max_point.z = center_point.z;
					break;
				case 3:
					child[triangle_quadrant[j]]->center_point.x = (center_point.x + min_point.x) / 2.f;
					child[triangle_quadrant[j]]->center_point.y = (center_point.y + max_point.y) / 2.f;
					child[triangle_quadrant[j]]->center_point.z = (center_point.z + max_point.z) / 2.f;

					child[triangle_quadrant[j]]->min_point.x = min_point.x;
					child[triangle_quadrant[j]]->min_point.y = center_point.y;
					child[triangle_quadrant[j]]->min_point.z = center_point.z;

					child[triangle_quadrant[j]]->max_point.x = center_point.x;
					child[triangle_quadrant[j]]->max_point.y = max_point.y;
					child[triangle_quadrant[j]]->max_point.z = max_point.z;
					break;
				case 4:
					child[triangle_quadrant[j]]->center_point.x = (center_point.x + max_point.x) / 2.f;
					child[triangle_quadrant[j]]->center_point.y = (center_point.y + min_point.y) / 2.f;
					child[triangle_quadrant[j]]->center_point.z = (center_point.z + min_point.z) / 2.f;

					child[triangle_quadrant[j]]->min_point.x = center_point.x;
					child[triangle_quadrant[j]]->min_point.y = min_point.y;
					child[triangle_quadrant[j]]->min_point.z = min_point.z;

					child[triangle_quadrant[j]]->max_point.x = max_point.x;
					child[triangle_quadrant[j]]->max_point.y = center_point.y;
					child[triangle_quadrant[j]]->max_point.z = center_point.z;
					break;
				case 5:
					child[triangle_quadrant[j]]->center_point.x = (center_point.x + max_point.x) / 2.f;
					child[triangle_quadrant[j]]->center_point.y = (center_point.y + min_point.y) / 2.f;
					child[triangle_quadrant[j]]->center_point.z = (center_point.z + max_point.z) / 2.f;

					child[triangle_quadrant[j]]->min_point.x = center_point.x;
					child[triangle_quadrant[j]]->min_point.y = min_point.y;
					child[triangle_quadrant[j]]->min_point.z = center_point.z;

					child[triangle_quadrant[j]]->max_point.x = max_point.x;
					child[triangle_quadrant[j]]->max_point.y = center_point.y;
					child[triangle_quadrant[j]]->max_point.z = max_point.z;
					break;
				case 6:
					child[triangle_quadrant[j]]->center_point.x = (center_point.x + max_point.x) / 2.f;
					child[triangle_quadrant[j]]->center_point.y = (center_point.y + max_point.y) / 2.f;
					child[triangle_quadrant[j]]->center_point.z = (center_point.z + min_point.z) / 2.f;

					child[triangle_quadrant[j]]->min_point.x = center_point.x;
					child[triangle_quadrant[j]]->min_point.y = center_point.y;
					child[triangle_quadrant[j]]->min_point.z = min_point.z;

					child[triangle_quadrant[j]]->max_point.x = max_point.x;
					child[triangle_quadrant[j]]->max_point.y = max_point.y;
					child[triangle_quadrant[j]]->max_point.z = center_point.z;
					break;
				case 7:
					child[triangle_quadrant[j]]->center_point = (center_point + max_point) / 2.f;
					child[triangle_quadrant[j]]->min_point = center_point;
					child[triangle_quadrant[j]]->max_point = max_point;
					break;
				default:
					break;
				}
			}
			++quadrant[triangle_quadrant[j]];
		}
		int max_quadrant = -1;
		int else_qudrant = -1;
		int max_num = -1;
		for (int j = 0; j < 8; ++j)
		{
			if (quadrant[j] > max_num)
			{
				if (max_quadrant != -1)
				{
					else_qudrant = max_quadrant;
				}
				max_quadrant = j;
				max_num = quadrant[j];
			}
			else if(quadrant[j]!=0)
			{
				else_qudrant = quadrant[j];
			}
		}
		if (max_num == 1)
		{
			continue;
		}
		else if (max_num == 2)
		{
			child[max_quadrant]->containing_vertices.push_back(containing_vertices[i]);
			child[max_quadrant]->containing_vertices.push_back(containing_vertices[i + 1]);
			child[max_quadrant]->containing_vertices.push_back(containing_vertices[i + 2]);
		}
		else
		{
			child[max_quadrant]->containing_vertices.push_back(containing_vertices[i]);
			child[max_quadrant]->containing_vertices.push_back(containing_vertices[i+1]);
			child[max_quadrant]->containing_vertices.push_back(containing_vertices[i+2]);
		}
	}

	for (int i = 0; i < 8; ++i)
	{
		if (child[i] != nullptr)
		{
			if (child[i]->containing_vertices.size() == 0)
			{
				delete child[i];
				child[i] = nullptr;
				continue;
			}

			child[i]->Compute_OctreeNode();
		}
	}

	containing_vertices.clear();

	return;
}

int OctreeNode::Judge_quadrant(glm::vec3& vec)
{
	bool is_plus_x = vec.x > center_point.x ? true : false;
	bool is_plus_y = vec.y > center_point.y ? true : false;
	bool is_plus_z = vec.z > center_point.z ? true : false;

	if (is_plus_x)
	{
		if (is_plus_y)
		{
			if (is_plus_z)
			{
				return 7;
			}
			else
			{
				return 6;
			}
		}
		else
		{
			if (is_plus_z)
			{
				return 5;
			}
			else
			{
				return 4;
			}
		}
	}
	else
	{
		if (is_plus_y)
		{
			if (is_plus_z)
			{
				return 3;
			}
			else
			{
				return 2;
			}
		}
		else
		{
			if (is_plus_z)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}

	return -1;
}

void OctreeNode::Draw_Octree(int height)
{
	for (int i = 0; i < 8; ++i)
	{
		if (child[i] != nullptr)
		{
			child[i]->Draw_Octree(height + 1);
		}
	}

	global_graphic->Draw_AABB(aabb_mesh);
	if (triangles_mesh.GetVerticesSize() != 0)
	{
		global_graphic->Draw_Triangles_line_Only(triangles_mesh, height);
	}
}
