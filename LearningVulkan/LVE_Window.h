#pragma once
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace LVE {

	class LVE_Window
	{
	public:
		LVE_Window(int width, int height, std::string name);
		~LVE_Window();
		LVE_Window(const LVE_Window&) = delete;
		LVE_Window& operator= (const LVE_Window&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(_window); }
		VkExtent2D getExtent() { return { static_cast<uint32_t>(_width), static_cast<uint32_t>(_height) }; }
		bool wasWindowResized() { return _frameBufferResized; }
		void resetWindowResizedFlag() { _frameBufferResized = false; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		GLFWwindow* _window;
		std::string _windowName;
		int _width;
		int _height;
		bool _frameBufferResized = false;

		void initWindow();
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	};

}