#pragma once
#include "LVE_Window.h"
#include "LVE_Device.h"
#include "LVE_SwapChain.h"
#include <memory>
#include <vector>
#include <cassert>

namespace LVE {

	class LVE_Renderer {
	public:
		LVE_Renderer(LVE_Window& window, LVE_Device& device);
		~LVE_Renderer();
		LVE_Renderer(const LVE_Renderer&) = delete;
		LVE_Renderer& operator= (const LVE_Renderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return _lveSwapChain->getRenderPass(); }
		float getAspectRatio() const { return _lveSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return _isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const { 
			assert(_isFrameStarted && "Cannot get command buffer when frame not in progress.");
			return _commandBuffers[_currentFrameIndex]; 
		}

		int getFrameIndex() const {
			assert(_isFrameStarted && "Cannot get frame index when frame not in progress.");
			return _currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		LVE_Window& _lveWindow;
		LVE_Device& _lveDevice;
		std::unique_ptr<LVE_SwapChain> _lveSwapChain;
		std::vector<VkCommandBuffer> _commandBuffers;

		uint32_t _currentImageIndex;
		int _currentFrameIndex;
		bool _isFrameStarted{ false };
	};

}