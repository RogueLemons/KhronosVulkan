#pragma once
#include "VulkanEngineWindow.h"
#include "VulkanEnginePipeline.h"

class FirstApp {
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	void run();

private:
	VulkanEngineWindow _veWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };

	// These paths only work when debugging
	VulkanEnginePipeline _vePipeline{"shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv" };
};