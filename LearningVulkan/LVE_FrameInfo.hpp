#pragma once
#include "LVE_Camera.h"

#include <vulkan/vulkan.h>

namespace LVE {

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		LVE_Camera& camera;
		VkDescriptorSet globalDescriptorSet;
	};

}
