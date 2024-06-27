#include "FirstApp.h"
#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace LVE {

	struct SimplePushConstantData {
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};
	
	FirstApp::FirstApp()
	{
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
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
			drawFrame();
		}

		vkDeviceWaitIdle(_veDevice.device());
	}

	void FirstApp::loadModels()
	{
		std::vector<VE_Model::Vertex> vertices{
			{ { 0.0f, -0.5f },	{ 1.0f, 0.0f, 0.0f } },
			{ { 0.5f, 0.5f },	{ 0.0f, 1.0f, 0.0f } },
			{ { -0.5f, 0.5f },	{ 0.0f, 0.0f, 1.0f } },
		};
		_veModel = std::make_unique<VE_Model>(_veDevice, vertices);
	}

	void FirstApp::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(_veDevice.device(), &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void FirstApp::createPipeline()
	{
		assert(_veSwapChain != nullptr && "Cannot create pipeline before swap chain.");
		assert(_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");

		PipelineConfigInfo pipelineConfig{};
		VE_Pipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = _veSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = _pipelineLayout;
		// These paths only work when debugging. TODO: Add auto-compile and file copy post-build events
		_vePipeline = std::make_unique<VE_Pipeline>(_veDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
	}

	void FirstApp::createCommandBuffers()
	{
		_commandBuffers.resize(_veSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _veDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

		if (vkAllocateCommandBuffers(_veDevice.device(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void FirstApp::freeCommandBuffers()
	{
		vkFreeCommandBuffers(_veDevice.device(), _veDevice.getCommandPool(), static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
		_commandBuffers.clear();
	}

	void FirstApp::drawFrame()
	{
		uint32_t imageIndex;
		auto result = _veSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to aqauire swap chain image!");
		}

		recordCommandBuffer(imageIndex);
		result = _veSwapChain->submitCommandBuffers(&_commandBuffers[imageIndex], &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _veWindow.wasWindowResized()) {
			_veWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image!");
		}
	}

	void FirstApp::recreateSwapChain()
	{
		auto extent = _veWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = _veWindow.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(_veDevice.device());

		if (_veSwapChain == nullptr) {
			_veSwapChain = std::make_unique<VE_SwapChain>(_veDevice, extent);
		}
		else {
			_veSwapChain = std::make_unique<VE_SwapChain>(_veDevice, extent, std::move(_veSwapChain));
			if (_veSwapChain->imageCount() != _commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
		
		createPipeline();
	}

	void FirstApp::recordCommandBuffer(int imageIndex)
	{
		static int frame = 0;
		frame = (frame + 1) % 100;

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(_commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _veSwapChain->getRenderPass();
		renderPassInfo.framebuffer = _veSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = _veSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(_commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(_veSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(_veSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, _veSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(_commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(_commandBuffers[imageIndex], 0, 1, &scissor);

		_vePipeline->bind(_commandBuffers[imageIndex]);
		_veModel->bind(_commandBuffers[imageIndex]);

		for (int j = 0; j < 4; j++) {
			SimplePushConstantData push{};
			push.offset = { -0.5f + frame * 0.02f, -0.4f + j * 0.25f };
			push.color = { 0.0f, 0.0f, 0.2f + 0.2f * j };

			vkCmdPushConstants(
				_commandBuffers[imageIndex],
				_pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push
			);

			_veModel->draw(_commandBuffers[imageIndex]);
		}

		vkCmdEndRenderPass(_commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(_commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer!");
		}
	}

}