#pragma once
#include "LVE_Window.h"
#include "LVE_Pipeline.h"
#include "LVE_Device.h"
#include "LVE_SwapChain.h"
#include "LVE_Model.h"
#include <memory>
#include <vector>

namespace LVE {

	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();
		FirstApp(const FirstApp&) = delete;
		FirstApp& operator= (const FirstApp&) = delete;

		void run();

	private:
		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		VE_Window _veWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		VE_Device _veDevice{ _veWindow };
		VE_SwapChain _veSwapChain{ _veDevice, _veWindow.getExtent() };
		std::unique_ptr<VE_Pipeline> _vePipeline;
		VkPipelineLayout _pipelineLayout;
		std::vector<VkCommandBuffer> _commandBuffers;
		std::unique_ptr<VE_Model> _veModel;
	};

}