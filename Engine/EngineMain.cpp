/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: EngineMain.cpp
Purpose: Main Class of assignment
Language: c++
Platform: visual studio 2019, window 10, 64bit
Project: s.shin_CS300_1
Author: Seongwook,Shin    s.shin  180003319
Creation date: 09/24/2019
End Header --------------------------------------------------------*/ 
#include "Engine.h"

int main()
{
	Engine* engine = new Engine;

	engine->Initailize();

	engine->MainLoop();

	engine->Quit();

	return 0;
}
