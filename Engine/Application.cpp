/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Application.cpp
Purpose: Implementation of Application class
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS300_1
Author: Seongwook,Shin    s.shin  180003319
Creation date: 09/24/2019
End Header --------------------------------------------------------*/ 
#include <iostream>
#include "Application.h"
#include "IMGUI/imgui.h"
#include "Input.h"

Application* APP = nullptr;

Application::Application():window(nullptr),Width(800), Height(800),graphic(nullptr)
{
	if (APP != nullptr)
	{
		std::cout<<"Error: APP is already exist\n";
		return;
	}
	APP = this;

    graphic = new Graphic();
}

Application::~Application()
{
}

void Application::Initialize()
{
	glfwSetErrorCallback(errorCallback);

	if (!glfwInit())
	{
		std::cerr << "Error: GLFW" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 1);

	window = glfwCreateWindow(Width, Height, "Graphic base physics", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetWindowCloseCallback(window, window_close_callback);

    graphic->Initialize();
}

void Application::Close()
{
    graphic->Close();
}

GLFWwindow * Application::Get_Window()
{
	return window;
}

Graphic* Application::Get_Graphic()
{
	return graphic;
}

void Application::Set_Window_Size(int width, int height)
{
	Width = width;
	Height = height;
}

void Application::Show_FramePerSecond(float fps)
{
    std::string fps_string = std::string(" fps : ") + std::to_string((int)std::roundf(fps));
    std::string showTitle = Title + fps_string;
    glfwSetWindowTitle(window, showTitle.c_str());
}

void Application::Set_Window_Title(const std::string& title)
{
    Title = title;
}

void errorCallback(int errorCode, const char* errorDescription)
{
	std::cerr << "Error: " << errorDescription << std::endl;
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	APP->Set_Window_Size(width, height);
    glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//ImGui Stuff
	ImGuiIO& io = ImGui::GetIO();
	if (action == GLFW_PRESS)
		io.KeysDown[key] = true;
	if (action == GLFW_RELEASE)
		io.KeysDown[key] = false;

	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

	Input::Set_Keyboard(static_cast<KEY_STATE>(action), key);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Input::Set_Mouse_Position(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	Input::Set_Mouse(static_cast<KEY_STATE>(action), button);
}

void window_close_callback(GLFWwindow* window)
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}
