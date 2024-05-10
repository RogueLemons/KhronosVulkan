#pragma once
#include "VulkanEngineWindow.h"

class FirstApp {
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	void run();

private:
	VulkanEngineWindow _veWindow = VulkanEngineWindow(WIDTH, HEIGHT, "Hello Vulkan!");
};