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

		LVE_Window _lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		LVE_Device _lveDevice{ _lveWindow };
		std::unique_ptr<LVE_SwapChain> _lveSwapChain;
		std::unique_ptr<LVE_Pipeline> _lvePipeline;
		VkPipelineLayout _pipelineLayout;
		std::vector<VkCommandBuffer> _commandBuffers;
		std::unique_ptr<LVE_Model> _lveModel;
	};

}