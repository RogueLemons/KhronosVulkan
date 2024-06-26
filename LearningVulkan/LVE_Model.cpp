#include "LVE_Model.h"
#include <cassert>
#include <cstring>

namespace LVE {

	VE_Model::VE_Model(VE_Device& device, const std::vector<Vertex>& vertices)
		: _veDevice(device)
	{
		createVertexBuffers(vertices);
	}

	VE_Model::~VE_Model()
	{
		vkDestroyBuffer(_veDevice.device(), _vertexBuffer, nullptr);
		vkFreeMemory(_veDevice.device(), _vertexBufferMemory, nullptr);
	}

	void VE_Model::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { _vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}

	void VE_Model::draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);
	}

	void VE_Model::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		_vertexCount = static_cast<uint32_t>(vertices.size());
		assert(_vertexCount >= 3 && "Vertex count must be at least 3.");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * _vertexCount;
		_veDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			_vertexBuffer,
			_vertexBufferMemory
		);

		void* data;
		vkMapMemory(_veDevice.device(), _vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(_veDevice.device(), _vertexBufferMemory);

	}

	std::vector<VkVertexInputBindingDescription> VE_Model::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> VE_Model::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}

}