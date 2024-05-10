#pragma once
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class VulkanEngineWindow
{
public:
	VulkanEngineWindow(int width, int height, std::string name);
	~VulkanEngineWindow();
	VulkanEngineWindow(const VulkanEngineWindow&) = delete;
	VulkanEngineWindow& operator= (const VulkanEngineWindow&) = delete;

	bool shouldClose() { return glfwWindowShouldClose(_window); }
private:
	GLFWwindow* _window;
	std::string _windowName;
	const int _width;
	const int _height;

	void initWindow();
};

