#include "FirstApp.h"
#include <stdexcept>
#include <array>
#include <cassert>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace LVE {

	struct SimplePushConstantData {
		glm::mat2 transform{ 1.0f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};
	
	FirstApp::FirstApp()
	{
		loadGameObjects();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	FirstApp::~FirstApp()
	{
		vkDestroyPipelineLayout(_lveDevice.device(), _pipelineLayout, nullptr);
	}

	void FirstApp::run()
	{
		while (!_lveWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(_lveDevice.device());
	}

	void FirstApp::loadGameObjects()
	{
		std::vector<LVE_Model::Vertex> vertices{
			{ { 0.0f, -0.5f },	{ 1.0f, 0.0f, 0.0f } },
			{ { 0.5f, 0.5f },	{ 0.0f, 1.0f, 0.0f } },
			{ { -0.5f, 0.5f },	{ 0.0f, 0.0f, 1.0f } },
		};
		auto lveModel = std::make_shared<LVE_Model>(_lveDevice, vertices);

		auto triangle = LVE_GameObject::createGameObject();
		triangle._model = lveModel;
		triangle._color = { 0.1f, 0.8f, 0.1f };
		triangle._transform2d.translation.x = 0.2f;
		triangle._transform2d.scale = { 2.0f, 0.5f };
		triangle._transform2d.rotation = 0.25f * glm::two_pi<float>();
		
		_gameObjects.push_back(std::move(triangle));
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
		if (vkCreatePipelineLayout(_lveDevice.device(), &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void FirstApp::createPipeline()
	{
		assert(_lveSwapChain != nullptr && "Cannot create pipeline before swap chain.");
		assert(_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");

		PipelineConfigInfo pipelineConfig{};
		LVE_Pipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = _lveSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = _pipelineLayout;
		// These paths only work when debugging. TODO: Add auto-compile and file copy post-build events
		_lvePipeline = std::make_unique<LVE_Pipeline>(_lveDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
	}

	void FirstApp::createCommandBuffers()
	{
		_commandBuffers.resize(_lveSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _lveDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

		if (vkAllocateCommandBuffers(_lveDevice.device(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void FirstApp::freeCommandBuffers()
	{
		vkFreeCommandBuffers(_lveDevice.device(), _lveDevice.getCommandPool(), static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
		_commandBuffers.clear();
	}

	void FirstApp::drawFrame()
	{
		uint32_t imageIndex;
		auto result = _lveSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to aqauire swap chain image!");
		}

		recordCommandBuffer(imageIndex);
		result = _lveSwapChain->submitCommandBuffers(&_commandBuffers[imageIndex], &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _lveWindow.wasWindowResized()) {
			_lveWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image!");
		}
	}

	void FirstApp::recreateSwapChain()
	{
		auto extent = _lveWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = _lveWindow.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(_lveDevice.device());

		if (_lveSwapChain == nullptr) {
			_lveSwapChain = std::make_unique<LVE_SwapChain>(_lveDevice, extent);
		}
		else {
			_lveSwapChain = std::make_unique<LVE_SwapChain>(_lveDevice, extent, std::move(_lveSwapChain));
			if (_lveSwapChain->imageCount() != _commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
		
		createPipeline();
	}

	void FirstApp::recordCommandBuffer(int imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(_commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _lveSwapChain->getRenderPass();
		renderPassInfo.framebuffer = _lveSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = _lveSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(_commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(_lveSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(_lveSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, _lveSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(_commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(_commandBuffers[imageIndex], 0, 1, &scissor);

		renderGameObjects(_commandBuffers[imageIndex]);

		vkCmdEndRenderPass(_commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(_commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer!");
		}
	}

	void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer)
	{
		_lvePipeline->bind(commandBuffer);

		for (auto& obj : _gameObjects) {
			obj._transform2d.rotation = glm::mod(obj._transform2d.rotation + 0.1f, glm::two_pi<float>());

			SimplePushConstantData push{};
			push.offset = obj._transform2d.translation;
			push.color = obj._color;
			push.transform = obj._transform2d.mat2();

			vkCmdPushConstants(
				commandBuffer,
				_pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push
			);
			
			obj._model->bind(commandBuffer);
			obj._model->draw(commandBuffer);
		}
	}

}