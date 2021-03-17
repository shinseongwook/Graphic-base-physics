/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Engine.h
Purpose: Definition of Engine class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS300_1
Author: Seongwook,Shin    s.shin  180003319
Creation date: 09/24/2019
End Header --------------------------------------------------------*/ 
#pragma once
#include <vector>
#include <string>
#include "Application.h"
#include "Input.h"

class Engine
{
public:
	Engine();
	~Engine();

	void Turn_Off_Debug_Draw();

	void MainLoop();

	void Initailize();
	void Quit();
private:
	bool draw_TD_AABB = false;
	bool draw_TD_sphere = false;

	bool draw_BU_AABB = false;
	bool draw_BU_sphere = false;

	bool draw_AABB = false;
	bool draw_centroid = false;
	bool draw_ritter = false;
	bool draw_pca = false;
	bool draw_larsson = false;
	bool draw_elipsoid = false;
	bool draw_obb = false;

	bool Is_Running;
	Application application;
	Graphic* graphic=nullptr;
	GLFWwindow* window;

	int obj_file_num=4;
	std::vector<std::string>obj_file_list;
};

static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_text = vector.at(idx).c_str();
	return true;
};