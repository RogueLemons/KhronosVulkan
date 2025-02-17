#pragma once

#include "LVE_Device.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>
#include <memory>

namespace LVE {

    class LVE_SwapChain {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        LVE_SwapChain(LVE_Device& deviceRef, VkExtent2D windowExtent);
        LVE_SwapChain(LVE_Device& deviceRef, VkExtent2D windowExtent, std::shared_ptr<LVE_SwapChain> previous);
        ~LVE_SwapChain();

        LVE_SwapChain(const LVE_SwapChain&) = delete;
        LVE_SwapChain& operator=(const LVE_SwapChain&) = delete;

        VkFramebuffer getFrameBuffer(int index) { return _swapChainFramebuffers[index]; }
        VkRenderPass getRenderPass() { return _renderPass; }
        VkImageView getImageView(int index) { return _swapChainImageViews[index]; }
        size_t imageCount() { return _swapChainImages.size(); }
        VkFormat getSwapChainImageFormat() { return _swapChainImageFormat; }
        VkExtent2D getSwapChainExtent() { return _swapChainExtent; }
        uint32_t width() { return _swapChainExtent.width; }
        uint32_t height() { return _swapChainExtent.height; }

        float extentAspectRatio() {
            return static_cast<float>(_swapChainExtent.width) / static_cast<float>(_swapChainExtent.height);
        }
        VkFormat findDepthFormat();

        VkResult acquireNextImage(uint32_t* imageIndex);
        VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

        bool compareSwapFormats(const LVE_SwapChain& swapChain) const {
            return swapChain._swapChainDepthFormat == _swapChainDepthFormat &&
                   swapChain._swapChainImageFormat == _swapChainImageFormat;
        }

    private:
        void init();
        void createSwapChain();
        void createImageViews();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        VkFormat _swapChainImageFormat;
        VkFormat _swapChainDepthFormat;
        VkExtent2D _swapChainExtent;

        std::vector<VkFramebuffer> _swapChainFramebuffers;
        VkRenderPass _renderPass;

        std::vector<VkImage> _depthImages;
        std::vector<VkDeviceMemory> _depthImageMemorys;
        std::vector<VkImageView> _depthImageViews;
        std::vector<VkImage> _swapChainImages;
        std::vector<VkImageView> _swapChainImageViews;

        LVE_Device& _lveDevice;
        VkExtent2D _windowExtent;

        VkSwapchainKHR _swapChain;
        std::shared_ptr<LVE_SwapChain> _oldSwapChain;

        std::vector<VkSemaphore> _imageAvailableSemaphores;
        std::vector<VkSemaphore> _renderFinishedSemaphores;
        std::vector<VkFence> _inFlightFences;
        std::vector<VkFence> _imagesInFlight;
        size_t _currentFrame = 0;
    };

}  // namespace LVE