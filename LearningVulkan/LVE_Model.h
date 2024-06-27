#pragma once
#include "LVE_Device.h"
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace LVE {
	
	class LVE_Model
	{
	public:

		struct Vertex {
			glm::vec2 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		LVE_Model(LVE_Device& device, const std::vector<Vertex>& vertices);
		~LVE_Model();
		LVE_Model(const LVE_Model&) = delete;
		LVE_Model& operator= (const LVE_Model&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);

		LVE_Device& _lveDevice;
		VkBuffer _vertexBuffer;
		VkDeviceMemory _vertexBufferMemory;
		uint32_t _vertexCount;
	};

}

