//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

//external
#include "glfw3.h"

namespace Graphics
{
	class Render
	{
	public:
		static inline GLFWwindow* window;

		static inline unsigned int currentWidth;
		static inline unsigned int currentHeight;

		static inline int windowedWidth;
		static inline int windowedHeight;

		static inline float aspectRatio;

		static void Initialize();

		static void UpdateAfterRescale(GLFWwindow* window, int width, int height);

		static void WindowLoop();
	};
}