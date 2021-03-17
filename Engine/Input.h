/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Input.h
Purpose: Definition of Input class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS300_1
Author: Seongwook,Shin    s.shin  180003319
Creation date: 09/24/2019
End Header --------------------------------------------------------*/ 
#pragma once

#include "Application.h"
#include <bitset>

enum KEY_STATE
{
	KEY_UP,
	KEY_TRIGGERED,
	KEY_DOWN
};

class Input
{
public:
	static void Initialize();

	static void Set_Keyboard(KEY_STATE state, int key_num);
	static bool Is_Pressed(int key_num);
	static bool Is_Triggered(int key_num);
	static bool Is_Released(int key_num);
	static void Reset_Triggered();

	static void Set_Mouse_Position(double x, double y);
	static glm::vec2 Get_Mouse_Position_diff();

	static void Set_Mouse(KEY_STATE state, int key_num);

private:
	static std::bitset<GLFW_KEY_LAST> k_pressed;
	static std::bitset<GLFW_KEY_LAST> k_triggered;
	static glm::vec2 m_pos;
	static glm::vec2 m_prev_pos;
};

