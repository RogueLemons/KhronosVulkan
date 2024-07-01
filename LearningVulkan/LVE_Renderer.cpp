#include "LVE_Renderer.h"
#include <stdexcept>
#include <array>
#include <cassert>

namespace LVE {

	LVE_Renderer::LVE_Renderer(LVE_Window& window, LVE_Device& device)
		: _lveWindow(window)
		, _lveDevice(device)
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	LVE_Renderer::~LVE_Renderer()
	{
		freeCommandBuffers();
	}

	VkCommandBuffer LVE_Renderer::beginFrame()
	{
		assert(!_isFrameStarted && "Cannot call beginFrame while already in progress.");

		auto result = _lveSwapChain->acquireNextImage(&_currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to aqauire swap chain image!");
		}

		_isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		return commandBuffer;
	}

	void LVE_Renderer::endFrame()
	{
		assert(_isFrameStarted && "Cannot call endFrame while frame is not in progress.");

		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer!");
		}

		auto result = _lveSwapChain->submitCommandBuffers(&commandBuffer, &_currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _lveWindow.wasWindowResized()) {
			_lveWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image!");
		}

		_isFrameStarted = false;
	}

	void LVE_Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(_isFrameStarted && "Cannot call beginSwapChainRenderPass if frame is not in progress.");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot begin render pass on command buffer from a different frame.");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _lveSwapChain->getRenderPass();
		renderPassInfo.framebuffer = _lveSwapChain->getFrameBuffer(_currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = _lveSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(_lveSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(_lveSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, _lveSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void LVE_Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(_isFrameStarted && "Cannot call endSwapChainRenderPass if frame is not in progress.");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot end render pass on command buffer from a different frame.");

		vkCmdEndRenderPass(commandBuffer);
	}

	void LVE_Renderer::createCommandBuffers()
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

	void LVE_Renderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(_lveDevice.device(), _lveDevice.getCommandPool(), static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
		_commandBuffers.clear();
	}

	void LVE_Renderer::recreateSwapChain()
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

		// createPipeline(); will come back in a moment
	}

}