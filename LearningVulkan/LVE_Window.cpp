#include "LVE_Window.h"

namespace LVE {

	VulkanEngineWindow::VulkanEngineWindow(int width, int height, std::string name)
		: _width(width), _height(height), _windowName(name)
	{
		initWindow();
	}

	VulkanEngineWindow::~VulkanEngineWindow()
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	void VulkanEngineWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		_window = glfwCreateWindow(_width, _height, _windowName.c_str(), nullptr, nullptr);
	}

}