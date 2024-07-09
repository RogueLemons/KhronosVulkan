#include "LVE_Model.h"
#include "LVE_Utils.hpp"
#include <cassert>
#include <cstring>
#include <unordered_map>

// Only allowed for one file
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std {

	template<>
	struct hash<LVE::LVE_Model::Vertex> {
		size_t operator()(const LVE::LVE_Model::Vertex& vertex) const {
			size_t seed = 0;
			LVE::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};

}

namespace LVE {

	LVE_Model::LVE_Model(LVE_Device& device, const Builder& builder)
		: _lveDevice(device)
	{
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);
	}

	LVE_Model::~LVE_Model()
	{
		vkDestroyBuffer(_lveDevice.device(), _vertexBuffer, nullptr);
		vkFreeMemory(_lveDevice.device(), _vertexBufferMemory, nullptr);

		if (_hasIndexBuffer) {
			vkDestroyBuffer(_lveDevice.device(), _indexBuffer, nullptr);
			vkFreeMemory(_lveDevice.device(), _indexBufferMemory, nullptr);
		}
	}

	std::unique_ptr<LVE_Model> LVE_Model::createModelFromFile(LVE_Device& device, const std::string& filepath)
	{
		Builder builder{};
		builder.loadModel(filepath);
		return std::make_unique<LVE_Model>(device, builder);
	}

	void LVE_Model::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { _vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (_hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void LVE_Model::draw(VkCommandBuffer commandBuffer)
	{
		if (_hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, _indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);
		}
	}

	void LVE_Model::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		_vertexCount = static_cast<uint32_t>(vertices.size());
		assert(_vertexCount >= 3 && "Vertex count must be at least 3.");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * _vertexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		_lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);

		void* data;
		vkMapMemory(_lveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(_lveDevice.device(), stagingBufferMemory);

		_lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			_vertexBuffer,
			_vertexBufferMemory
		);

		_lveDevice.copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

		vkDestroyBuffer(_lveDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(_lveDevice.device(), stagingBufferMemory, nullptr);
	}

	void LVE_Model::createIndexBuffers(const std::vector<uint32_t>& indices)
	{
		_indexCount = static_cast<uint32_t>(indices.size());
		_hasIndexBuffer = _indexCount > 0;

		if (!_hasIndexBuffer) {
			return;
		}

		VkDeviceSize bufferSize = sizeof(indices[0]) * _indexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		_lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);

		void* data;
		vkMapMemory(_lveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(_lveDevice.device(), stagingBufferMemory);

		_lveDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			_indexBuffer,
			_indexBufferMemory
		);

		_lveDevice.copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

		vkDestroyBuffer(_lveDevice.device(), stagingBuffer, nullptr);
		vkFreeMemory(_lveDevice.device(), stagingBufferMemory, nullptr);
	}

	std::vector<VkVertexInputBindingDescription> LVE_Model::Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> LVE_Model::Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

		return attributeDescriptions;
	}

	void LVE_Model::Builder::loadModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				if (index.vertex_index >= 0) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};
				}

				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.normals[2 * index.normal_index + 0],
						attrib.normals[2 * index.normal_index + 1],
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}

	}

}