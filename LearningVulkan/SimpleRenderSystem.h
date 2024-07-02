#pragma once
#include "LVE_Pipeline.h"
#include "LVE_Device.h"
#include "LVE_GameObject.h"
#include "LVE_Camera.h"
#include <memory>
#include <vector>

namespace LVE {

	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(LVE_Device& device, VkRenderPass renderPass);
		~SimpleRenderSystem();
		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator= (const SimpleRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LVE_GameObject>& gameObjects, const LVE_Camera& camera);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		LVE_Device& _lveDevice;
		std::unique_ptr<LVE_Pipeline> _lvePipeline;
		VkPipelineLayout _pipelineLayout;
	};

}