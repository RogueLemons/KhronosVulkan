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
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);

		VE_Window _veWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		VE_Device _veDevice{ _veWindow };
		std::unique_ptr<VE_SwapChain> _veSwapChain;
		std::unique_ptr<VE_Pipeline> _vePipeline;
		VkPipelineLayout _pipelineLayout;
		std::vector<VkCommandBuffer> _commandBuffers;
		std::unique_ptr<VE_Model> _veModel;
	};

}