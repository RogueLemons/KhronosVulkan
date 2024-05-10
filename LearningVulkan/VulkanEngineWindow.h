#pragma once
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class VulkanEngineWindow
{
public:
	VulkanEngineWindow(int width, int height, std::string name);
	~VulkanEngineWindow();
private:
	GLFWwindow* _window;
	std::string _windowName;
	const int _width;
	const int _height;

	void initWindow();
};

