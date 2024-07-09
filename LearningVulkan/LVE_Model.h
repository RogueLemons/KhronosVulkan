#pragma once
#include "LVE_Device.h"
#include <vector>
#include <memory>
#include <string>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace LVE {
	
	class LVE_Model
	{
	public:

		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator ==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

		LVE_Model(LVE_Device& device, const Builder& builder);
		~LVE_Model();
		LVE_Model(const LVE_Model&) = delete;
		LVE_Model& operator= (const LVE_Model&) = delete;

		static std::unique_ptr<LVE_Model> createModelFromFile(LVE_Device& device, const std::string& filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		LVE_Device& _lveDevice;

		VkBuffer _vertexBuffer;
		VkDeviceMemory _vertexBufferMemory;
		uint32_t _vertexCount;

		bool _hasIndexBuffer = false;
		VkBuffer _indexBuffer;
		VkDeviceMemory _indexBufferMemory;
		uint32_t _indexCount;
	};

}

