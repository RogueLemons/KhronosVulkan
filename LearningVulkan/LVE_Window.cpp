#include "LVE_Window.h"

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

	void VE_Window::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		_window = glfwCreateWindow(_width, _height, _windowName.c_str(), nullptr, nullptr);
	}

}