#pragma once
#include "LVE_Pipeline.h"
#include "LVE_Device.h"
#include "LVE_GameObject.h"
#include "LVE_Camera.h"
#include "LVE_FrameInfo.hpp"
#include <memory>
#include <vector>

namespace LVE {

	class PointLightSystem {
	public:
		PointLightSystem(LVE_Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();
		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator= (const PointLightSystem&) = delete;

		void render(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		LVE_Device& _lveDevice;
		std::unique_ptr<LVE_Pipeline> _lvePipeline;
		VkPipelineLayout _pipelineLayout;
	};

}