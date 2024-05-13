#pragma once
#include <string>
#include <vector>

namespace LVE {

	class VE_Pipeline
	{
	public:
		VE_Pipeline(const std::string& vertFilepath, const std::string& fragFilepath);
	private:
		static std::vector<char> readFile(const std::string& filepath);
		void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath);
	};

}

