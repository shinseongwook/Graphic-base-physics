/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Engine.cpp
Purpose: Implementation of Engine class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS300_1
Author: Seongwook,Shin    s.shin  180003319
Creation date: 09/24/2019
End Header --------------------------------------------------------*/ 
#include <filesystem>
#include "Engine.h"
#include "Graphic.h"
#include "Mesh.h"
#include "BoundingVolume.h"
#include "TopDownTree.h"
#include "BottomUpTree.h"
#include "Octree.h"
#include "BSPTree.h"
#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_glfw_gl3.h"

Engine::Engine() :Is_Running(true), obj_file_num(0), window(nullptr),obj_file_list(),graphic(nullptr)
{
}

Engine::~Engine()
{
}

void Engine::Turn_Off_Debug_Draw()
{
	draw_TD_AABB = false;
	draw_TD_sphere = false;

	draw_BU_AABB = false;
	draw_BU_sphere = false;

	draw_AABB = false;
	draw_centroid = false;
	draw_ritter = false;
	draw_larsson = false;
	draw_pca = false;
	draw_elipsoid = false;
	draw_obb = false;
}

void Engine::MainLoop()
{
	std::vector<Mesh> plant_container;
	//Mesh center_object;
	
	int num_of_file = 0;

	for (std::filesystem::directory_entry folder_list : std::filesystem::directory_iterator("models/Powerplant4"))
	{
		for (const auto& part_folder_list : std::filesystem::directory_iterator{ folder_list })
		{
			for (const auto& file_list : std::filesystem::directory_iterator{ part_folder_list })
			{
				plant_container.emplace_back();
				std::filesystem::path file_path = file_list.path();
				std::string path_string = file_path.u8string();
				plant_container[num_of_file].Load_Plyfile(path_string);
				//center_object.Load_Plyfile(path_string);
				num_of_file++;
			}
		}
	}

	std::vector<glm::vec3> all_vertices;
	Octree octree;
	octree.head_node = new OctreeNode;
	std::vector<BSPTree>BSPTree_per_Object;
	
	for (auto& iter : plant_container)
	{
		iter.Calibrating_Vertexcoord();
		iter.Compute_BoundingVolume();
		BSPTree_per_Object.emplace_back();
		BSPTree_per_Object.back().head_node = new BSPTreeNode;
		for (auto& iter2 : iter.GetVertices())
		{
			all_vertices.push_back(iter2);
		}
		for (auto& iter2 : iter.GetIndices())
		{
			octree.head_node->containing_vertices.push_back(iter.GetVertices()[iter2]);
			BSPTree_per_Object.back().head_node->containing_vertices.push_back(iter.GetVertices()[iter2]);
		}
		BSPTree_per_Object.back().Compute_BSPTree();
	}
	octree.Compute_Octree();

	glm::vec3 median=glm::vec3(0.f);
	for (auto& iter : all_vertices)
	{
		median += iter;
	}
	median /= all_vertices.size();

	TopDownTree topdown_AABB_Tree;
	TopDownTree topdown_BS_Tree;
	TopDownNode headnode_AABB;
	headnode_AABB.containing_vertices = all_vertices;
	headnode_AABB.median_point = median;
	topdown_AABB_Tree.head_node = &headnode_AABB;
	topdown_AABB_Tree.Compute_AABB();
	TopDownNode headnode_BS;
	headnode_BS.containing_vertices = all_vertices;
	headnode_BS.median_point = median;
	topdown_BS_Tree.head_node = &headnode_BS;
	topdown_BS_Tree.Compute_BS();


	BottomUpTree bottomup_AABB_Tree;
	BottomUpTree bottomup_BS_Tree;
	for (auto& iter : plant_container)
	{
		BottomUpNode* inserting_node = new BottomUpNode;
		inserting_node->containing_vertices = iter.GetVertices();
		BottomUpNode* inserting_node2 = new BottomUpNode;
		inserting_node2->containing_vertices = iter.GetVertices();

		bottomup_AABB_Tree.node_vector.push_back(inserting_node);
		bottomup_BS_Tree.node_vector.push_back(inserting_node2);
	}
	bottomup_AABB_Tree.Compute_AABB();
	bottomup_BS_Tree.Compute_BS();


	Mesh rotate_object[8];
	float angle_diff = glm::pi<float>() / 8.f * 2.f;
	for (int i = 0; i < 8; ++i)
	{
		rotate_object[i].MakeSphere(1.0f, 20);
		rotate_object[i].SetTranslate(glm::vec3(sin(angle_diff * i) * 0.6f, 0, cos(angle_diff * i) * 0.6f));
		rotate_object[i].SetScale(glm::vec3(0.045f, 0.045f, 0.045f));
	}

	bool rotate = true;
	bool draw_vertex_normal = false;
	bool draw_face_normal = false;
	bool copy_depth_info = true;

	bool assignment3_draw = false;
	bool draw_octree = false;
	bool draw_BSPtree = false;

	int previous_obj_num = 0;
	float dt = 0;

	int TD_bounding_sphere_height = 0;
	int TD_aabb_height = 0;

	int BU_bounding_sphere_height = 0;
	int BU_aabb_height = 0;

	SHADER_PROGRAM_TYPE current_program = SHADER_PROGRAM_TYPE::PHONG_SHADING_SHADER;
	int num_lights = 8;
	graphic->Change_Lights_Number(num_lights);

	while(Is_Running)
	{
		if (Input::Is_Triggered(GLFW_KEY_ESCAPE))
			break;
		graphic->Clear_Buffer();

		glfwPollEvents();
		glfwMakeContextCurrent(window);
		
		if (rotate)
			dt += 0.016f;

		if (assignment3_draw == false)
		{
			for (auto& iter : plant_container)
			{
				graphic->Draw_Face_to_Gbuffer(iter);
			}
		}
		else
		{
			int tri_draw = 1;
			int bsp_draw = 1;
		}

		graphic->RenderQuad();

		ImGui_ImplGlfwGL3_NewFrame();

		ImGui::Begin("Graphic base physics example");
		if (ImGui::Button("Recompile Shader"))
		{
			graphic->Recompile();
		}
		if (ImGui::CollapsingHeader("Object information"))
		{
			ImGui::Checkbox("Copy_Depth_Info", &copy_depth_info);

			ImGui::NewLine();
			if (ImGui::Checkbox("Mix_all", &graphic->light[0].is_mix_all))
			{
				assignment3_draw = false;
				for (int i = 0; i < num_lights; ++i)
				{
					graphic->light[i].is_mix_all = true;
					graphic->light[i].is_position = false;
					graphic->light[i].is_normal = false;
					graphic->light[i].is_albedo = false;
				}
			}
			if (ImGui::Checkbox("position_base", &graphic->light[0].is_position))
			{
				assignment3_draw = false;
				for (int i = 0; i < num_lights; ++i)
				{
					graphic->light[i].is_mix_all = false;
					graphic->light[i].is_position = true;
					graphic->light[i].is_normal = false;
					graphic->light[i].is_albedo = false;
				}
			}
			if (ImGui::Checkbox("normal_base", &graphic->light[0].is_normal))
			{
				assignment3_draw = false;
				for (int i = 0; i < num_lights; ++i)
				{
					graphic->light[i].is_mix_all = false;
					graphic->light[i].is_position = false;
					graphic->light[i].is_normal = true;
					graphic->light[i].is_albedo = false;
				}
			}
			if (ImGui::Checkbox("albedo_base", &graphic->light[0].is_albedo))
			{
				assignment3_draw = false;
				for (int i = 0; i < num_lights; ++i)
				{
					graphic->light[i].is_mix_all = false;
					graphic->light[i].is_position = false;
					graphic->light[i].is_normal = false;
					graphic->light[i].is_albedo = true;
				}
			}
			ImGui::NewLine();
			if (ImGui::Checkbox("Draw_vertex_normal", &draw_vertex_normal))
			{
				assignment3_draw = false;;
			}
			if (draw_vertex_normal)
			{
				//draw vn;
				for (auto& iter : plant_container)
				{
					graphic->Draw_Vertex_Normal(iter);
				}

				//graphic->Draw_Vertex_Normal(center_object);
			}
			if (ImGui::Checkbox("Draw_face_normal", &draw_face_normal))
			{
				assignment3_draw = false;
			}
			if (draw_face_normal)
			{
				//draw fn;
				for (auto& iter : plant_container)
				{
					graphic->Draw_Face_Normal(iter);
				}

				//graphic->Draw_Face_Normal(center_object);
			}
		}
		if (copy_depth_info)
		{
			graphic->Copy_Depth_to_Defaultbuffer();
		}

		if (ImGui::CollapsingHeader("Graphic base physics"))
		{
			if (ImGui::Checkbox("Draw AABB", &draw_AABB))
			{
				assignment3_draw = false;
				Turn_Off_Debug_Draw();
				draw_AABB = true;
			}
			if (ImGui::Checkbox("Draw centroid Sphere", &draw_centroid))
			{
				assignment3_draw = false;
				Turn_Off_Debug_Draw();
				draw_centroid = true;
			}
			if (ImGui::Checkbox("Draw ritter Sphere", &draw_ritter))
			{
				assignment3_draw = false;
				Turn_Off_Debug_Draw();
				draw_ritter = true;
			}
			if (ImGui::Checkbox("Draw larsson Sphere", &draw_larsson))
			{
				assignment3_draw = false;
				Turn_Off_Debug_Draw();
				draw_larsson = true;
			}
			if (ImGui::Checkbox("Draw PCA Sphere", &draw_pca))
			{
				assignment3_draw = false;
				Turn_Off_Debug_Draw();
				draw_pca = true;
			}
			if (ImGui::Checkbox("Draw elipsoid", &draw_elipsoid))
			{
				assignment3_draw = false;
				Turn_Off_Debug_Draw();
				draw_elipsoid = true;
			}
			if (ImGui::Checkbox("Draw obb", &draw_obb))
			{
				assignment3_draw = false;
				Turn_Off_Debug_Draw();
				draw_obb = true;
			}

			if (ImGui::Checkbox("Draw Topdown AABB", &draw_TD_AABB))
			{
				assignment3_draw = false;
				Turn_Off_Debug_Draw();
				draw_TD_AABB = true;
			}
			if (ImGui::Checkbox("Draw Topdown Sphere", &draw_TD_sphere))
			{
				assignment3_draw = false;
				Turn_Off_Debug_Draw();
				draw_TD_sphere = true;
			}
			if (ImGui::Checkbox("Draw BottomUp AABB", &draw_BU_AABB))
			{
				assignment3_draw = false;
				Turn_Off_Debug_Draw();
				draw_BU_AABB = true;
			}
			if (ImGui::Checkbox("Draw BottomUp Sphere", &draw_BU_sphere))
			{
				assignment3_draw = false;
				Turn_Off_Debug_Draw();
				draw_BU_sphere = true;
			}

			if (ImGui::Button("Draw Nothing"))
			{
				Turn_Off_Debug_Draw();
			}
			ImGui::Text("TopDown AABB_height");
			ImGui::SliderInt(" ", &TD_aabb_height, 0, 6);
			ImGui::Text("TopDown BS_height");
			ImGui::SliderInt("  ", &TD_bounding_sphere_height, 0, 6);
			ImGui::Text("BottomUp AABB_height");
			ImGui::SliderInt("   ", &BU_aabb_height, 0, bottomup_AABB_Tree.longest_height-1);
			ImGui::Text("BottomUp BS_height");
			ImGui::SliderInt("    ", &BU_bounding_sphere_height, 0, bottomup_BS_Tree.longest_height-1);
		}

		if (ImGui::CollapsingHeader("Octree/BSP Tree"))
		{
			if (ImGui::Checkbox("Draw Octree", &draw_octree))
			{
				Turn_Off_Debug_Draw();
				assignment3_draw = true;
				draw_octree = true;
				draw_BSPtree = false;
			}
			if (ImGui::Checkbox("Draw BSP Tree", &draw_BSPtree))
			{
				Turn_Off_Debug_Draw();
				assignment3_draw = true;
				draw_BSPtree = true;
				draw_octree = false;
			}
			if (ImGui::Button("Turn off tree draw"))
			{
				assignment3_draw = false;
				draw_BSPtree = false;
				draw_octree = false;
			}
		}

		if (!assignment3_draw)
		{
			for (auto& iter : plant_container)
			{
				if (draw_AABB)
				{
					iter.bounding_volume->Draw_AABB();
				}
				else if (draw_centroid)
				{
					iter.bounding_volume->Draw_Centroid_Sphere();
				}
				else if (draw_ritter)
				{
					iter.bounding_volume->Draw_Ritter_Sphere();
				}
				else if (draw_larsson)
				{
					iter.bounding_volume->Draw_Larsson_Sphere();
				}
				else if (draw_pca)
				{
					iter.bounding_volume->Draw_PCA_Sphere();
				}
				else if (draw_elipsoid)
				{
					iter.bounding_volume->Draw_PCA_ellipsoid();
				}
				else if (draw_obb)
				{
					iter.bounding_volume->Draw_OBB_PCA();
				}
			}
			if (draw_TD_AABB)
			{
				topdown_AABB_Tree.Draw_AABB(TD_aabb_height + 1);
			}
			else if (draw_TD_sphere)
			{
				topdown_BS_Tree.Draw_Sphere(TD_bounding_sphere_height + 1);
			}
			else if (draw_BU_AABB)
			{
				bottomup_AABB_Tree.Draw_AABB(BU_aabb_height + 1);
			}
			else if (draw_BU_sphere)
			{
				bottomup_BS_Tree.Draw_Sphere(BU_bounding_sphere_height + 1);
			}
		}
		else
		{
			if (draw_octree)
			{
				octree.Draw_Octree(0);
			}
			else if (draw_BSPtree)
			{
				for (auto& iter : BSPTree_per_Object)
				{
					iter.Draw_BSPTree(0);
				}
			}
		}

		for (int i = 0; i < num_lights; ++i)
		{
			rotate_object[i].SetTranslate(glm::vec3(sin(dt + angle_diff * i) * 0.6f, 0, cos(dt + angle_diff * i) * 0.6f));
			graphic->Setting_LightPosDir(i, glm::vec3(sin(dt + angle_diff * i) * 0.6f, 0, cos(dt + angle_diff * i) * 0.6f));
			graphic->Draw_Basic_Face(rotate_object[i], i);
		}


		ImGui::End();

		ImGui::Render();

		glfwMakeContextCurrent(window);
		glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);

		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(APP->Get_Window());

		if (Input::Is_Pressed(GLFW_KEY_A))
		{
			graphic->camera.ProcessMouseMovement(-100.0f,0);
		}
		if (Input::Is_Pressed(GLFW_KEY_D))
		{
			graphic->camera.ProcessMouseMovement(100.0f, 0);
		}
		if (Input::Is_Pressed(GLFW_KEY_W))
		{
			graphic->camera.ProcessKeyboard(Camera_Movement::BACKWARD, 0.016f);
		}
		if (Input::Is_Pressed(GLFW_KEY_S))
		{
			graphic->camera.ProcessKeyboard(Camera_Movement::FORWARD, 0.016f);
		}
	}
}

void Engine::Initailize()
{
	Input::Initialize();
	application.Initialize();
	window = application.Get_Window();

	//Gui setup
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	obj_file_list.push_back("4Sphere");
	obj_file_list.push_back("bunny");
	obj_file_list.push_back("bunny_high_poly");
	obj_file_list.push_back("cube");
	obj_file_list.push_back("cube2");
	obj_file_list.push_back("cup");
	obj_file_list.push_back("lucy_princeton");
	obj_file_list.push_back("quad");
	obj_file_list.push_back("rhino");
	obj_file_list.push_back("sphere");
	obj_file_list.push_back("sphere_modified");
	obj_file_list.push_back("starwars1");
	obj_file_list.push_back("triangle");

	graphic = application.Get_Graphic();

	application.Set_Window_Title("CS300 Assignment3");
}

void Engine::Quit()
{
}
