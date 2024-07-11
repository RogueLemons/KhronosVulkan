#pragma once
#include "LVE_Camera.h"
#include "LVE_GameObject.h"

#include <vulkan/vulkan.h>

namespace LVE {

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		LVE_Camera& camera;
		VkDescriptorSet globalDescriptorSet;
		LVE_GameObject::Map& gameObjects;
	};

}
