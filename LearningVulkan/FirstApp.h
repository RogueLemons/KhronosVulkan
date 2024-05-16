#pragma once
#include "LVE_Window.h"
#include "LVE_Pipeline.h"
#include "LVE_Device.h"

namespace LVE {

	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		VE_Window _veWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		VE_Device _veDevice{ _veWindow };

		// These paths only work when debugging
		VE_Pipeline _vePipeline{ _veDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", VE_Pipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT) };
	};

}