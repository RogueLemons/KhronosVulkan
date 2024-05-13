#pragma once
#include "LVE_Device.h"
#include <string>
#include <vector>

namespace LVE {

	struct PipelineConfigInfo {
	};

	class VE_Pipeline
	{
	public:
		VE_Pipeline(
			VE_Device& device,
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo);

		~VE_Pipeline() {}

		VE_Pipeline(const VE_Pipeline&) = delete;
		void operator= (const VE_Pipeline&) = delete;

		static PipelineConfigInfo defaultPiplelineConfigInfo(uint32_t width, uint32_t height);

	private:
		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(
			const std::string& vertFilepath,
			const std::string& fragFilepath,
			const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		VE_Device& _veDevice;
		VkPipeline _graphicPipeline;
		VkShaderModule _vertShaderModule;
		VkShaderModule _fragShaderModule;
	};

}

