/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Input.cpp
Purpose: Implementation of Input class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS300_1
Author: Seongwook,Shin    s.shin  180003319
Creation date: 09/24/2019
End Header --------------------------------------------------------*/ 
#include "Input.h"

std::bitset<GLFW_KEY_LAST> Input::k_pressed;
std::bitset<GLFW_KEY_LAST> Input::k_triggered;
glm::vec2 Input::m_pos;
glm::vec2 Input::m_prev_pos;

void Input::Initialize()
{
	k_pressed.reset();
	k_triggered.reset();
	m_pos = { 0,0 };
	m_prev_pos = { 0,0 };
}

void Input::Set_Keyboard(KEY_STATE state, int key_num)
{
	switch (state)
	{
	case KEY_UP:
		k_pressed[key_num] = false;
		break;
	case KEY_TRIGGERED:
		k_triggered[key_num] = true;
		k_pressed[key_num] = true;
		break;
	case KEY_DOWN:
		k_pressed[key_num] = true;
		break;
	}
}

bool Input::Is_Pressed(int key_num)
{
	return k_pressed[key_num];
}

bool Input::Is_Triggered(int key_num)
{
	return k_triggered[key_num];
}

bool Input::Is_Released(int key_num)
{
	return !k_pressed[key_num];
}

void Input::Reset_Triggered()
{
	k_triggered.reset();
}

void Input::Set_Mouse_Position(double x, double y)
{
	m_prev_pos = m_pos;
	m_pos.x = static_cast<float>(x)-APP->Width/2.f;
	m_pos.y = -static_cast<float>(y)+APP->Height/2.f;
}

glm::vec2 Input::Get_Mouse_Position_diff()
{
	return m_pos - m_prev_pos;
}

void Input::Set_Mouse(KEY_STATE state, int key_num)
{
	switch (state)
	{
	case KEY_UP:
		k_pressed[key_num] = false;
		break;
	case KEY_TRIGGERED:
		if (k_pressed[key_num] == false)
			k_triggered[key_num] = true;
		k_pressed[key_num] = true;
		break;
	}
}