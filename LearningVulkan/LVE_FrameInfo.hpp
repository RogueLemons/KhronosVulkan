#pragma once
#include "LVE_Camera.h"
#include "LVE_GameObject.h"

#include <vulkan/vulkan.h>

namespace LVE {

	constexpr auto MAX_LIGHTS = 10;

	struct PointLight {
		glm::vec4 position{};	// ignore w
		glm::vec4 color{};		// w is intensity
	};

	struct GlobalUbo {
		glm::mat4 projection{ 1.0f };
		glm::mat4 view{ 1.0f };
		glm::vec4 ambientLightColor{ 1.0f, 1.0f, 1.0f, 0.02f }; // w is intensity
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		LVE_Camera& camera;
		VkDescriptorSet globalDescriptorSet;
		LVE_GameObject::Map& gameObjects;
	};

}
