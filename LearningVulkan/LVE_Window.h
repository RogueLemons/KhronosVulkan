#pragma once
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace LVE {

	class VE_Window
	{
	public:
		VE_Window(int width, int height, std::string name);
		~VE_Window();
		VE_Window(const VE_Window&) = delete;
		VE_Window& operator= (const VE_Window&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(_window); }
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		GLFWwindow* _window;
		std::string _windowName;
		const int _width;
		const int _height;

		void initWindow();
	};

}