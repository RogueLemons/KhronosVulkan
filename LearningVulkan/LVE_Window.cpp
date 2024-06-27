#include "LVE_Window.h"
#include <stdexcept>

namespace LVE {

	LVE_Window::LVE_Window(int width, int height, std::string name)
		: _width(width), _height(height), _windowName(name)
	{
		initWindow();
	}

	LVE_Window::~LVE_Window()
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	void LVE_Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, _window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface.");
		}
	}

	void LVE_Window::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		_window = glfwCreateWindow(_width, _height, _windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(_window, this);
		glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
	}

	void LVE_Window::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto lveWindow = reinterpret_cast<LVE_Window*>(glfwGetWindowUserPointer(window));
		lveWindow->_frameBufferResized = true;
		lveWindow->_width = width;
		lveWindow->_height = height;
	}

}