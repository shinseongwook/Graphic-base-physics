/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Application.h
Purpose: Definition of Application class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS300_1
Author: Seongwook,Shin    s.shin  180003319
Creation date: 09/24/2019
End Header --------------------------------------------------------*/ 
#pragma once
#include <string>
#include <GLFW/glfw3.h>
#include "Graphic.h"

class State;

class Application
{
	friend class Input;
public:
	Application();
	~Application();
	virtual void Initialize();
    virtual void Close();

	GLFWwindow* Get_Window();
	Graphic* Get_Graphic();

	void Set_Window_Size(int width,int height);
    void Show_FramePerSecond(float fps);

    void Set_Window_Title(const std::string& title);

	int Width;
	int Height;
private:
	GLFWwindow * window;
    std::string Title;

    Graphic * graphic;
};

void errorCallback(int errorCode, const char* errorDescription);
void window_size_callback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void window_close_callback(GLFWwindow* window);

extern Application* APP;
