#include "LVE_Window.h"
#include <stdexcept>

namespace LVE {

	VE_Window::VE_Window(int width, int height, std::string name)
		: _width(width), _height(height), _windowName(name)
	{
		initWindow();
	}

	VE_Window::~VE_Window()
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	void VE_Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, _window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface.");
		}
	}

	void VE_Window::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		_window = glfwCreateWindow(_width, _height, _windowName.c_str(), nullptr, nullptr);
	}

}