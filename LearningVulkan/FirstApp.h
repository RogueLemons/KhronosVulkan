#pragma once
#include "LVE_Window.h"
#include "LVE_Pipeline.h"

class FirstApp {
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	void run();

private:
	LVE::VulkanEngineWindow _veWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };

	// These paths only work when debugging
	LVE::VulkanEnginePipeline _vePipeline{"shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv" };
};