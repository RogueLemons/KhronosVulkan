#include "SimpleRenderSystem.h"
#include <stdexcept>
#include <array>
#include <cassert>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace LVE {

	struct SimplePushConstantData {
		glm::mat4 transform{ 1.0f };
		alignas(16) glm::vec3 color;
	};

	SimpleRenderSystem::SimpleRenderSystem(LVE_Device& device, VkRenderPass renderPass)
		: _lveDevice(device)
	{
		createPipelineLayout();
		createPipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem()
	{
		vkDestroyPipelineLayout(_lveDevice.device(), _pipelineLayout, nullptr);
	}

	void SimpleRenderSystem::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(_lveDevice.device(), &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void SimpleRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");

		PipelineConfigInfo pipelineConfig{};
		LVE_Pipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = _pipelineLayout;
		_lvePipeline = std::make_unique<LVE_Pipeline>(_lveDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
	}

	void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LVE_GameObject>& gameObjects)
	{
		_lvePipeline->bind(commandBuffer);

		for (auto& obj : gameObjects) {
			obj._transform.rotation.y = glm::mod(obj._transform.rotation.y + 0.01f, glm::two_pi<float>());
			obj._transform.rotation.x = glm::mod(obj._transform.rotation.x + 0.005f, glm::two_pi<float>());

			SimplePushConstantData push{};
			push.color = obj._color;
			push.transform = obj._transform.mat4();

			vkCmdPushConstants(
				commandBuffer,
				_pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push
			);

			obj._model->bind(commandBuffer);
			obj._model->draw(commandBuffer);
		}
	}

}