#include "FirstApp.h"
#include <stdexcept>

namespace LVE {
	
	FirstApp::FirstApp()
	{
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	FirstApp::~FirstApp()
	{
		vkDestroyPipelineLayout(_veDevice.device(), _pipelineLayout, nullptr);
	}

	void FirstApp::run()
	{
		while (!_veWindow.shouldClose()) {
			glfwPollEvents();
		}
	}

	void FirstApp::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(_veDevice.device(), &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void FirstApp::createPipeline()
	{
		PipelineConfigInfo pipelineConfig{};
		VE_Pipeline::defaultPipelineConfigInfo(pipelineConfig, _veSwapChain.width(), _veSwapChain.height());
		pipelineConfig.renderPass = _veSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = _pipelineLayout;
		// These paths only work when debugging
		_vePipeline = std::make_unique<VE_Pipeline>(_veDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
	}

	void FirstApp::createCommandBuffers()
	{
	}

	void FirstApp::drawFrame()
	{
	}

}