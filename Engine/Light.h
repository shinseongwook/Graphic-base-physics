/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Light.h
Purpose: Definition of Light class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS300_2
Author: Seongwook,Shin    s.shin  180003319
Creation date: 11/10/2019
End Header --------------------------------------------------------*/
#pragma once
#include "glm/vec3.hpp"

class Graphic;

struct Light
{
public:
	Light();
	Light(int num, Graphic* ptr);

	void WritetoShader();


	Graphic* graphic = nullptr;;


	int light_num = 0;
	bool is_mix_all= true;
	bool is_position = false;
	bool is_normal = false;
	bool is_albedo = false;

	glm::vec3 pos;
	glm::vec3 direction;

	float cutOff=10.f;
	float outerCutOff=30.0f;
	float falloff=10.5f;

	glm::vec3 ambient = glm::vec3(0.01f, 0.01f, 0.01f);
	glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	glm::vec3 specular = glm::vec3(0.03f, 0.03f, 0.03f);

};

