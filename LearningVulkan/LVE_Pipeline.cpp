#include "LVE_Pipeline.h"

#include <fstream>
#include <stdexcept>
#include <iostream>

namespace LVE {

    VE_Pipeline::VE_Pipeline(const std::string& vertFilepath, const std::string& fragFilepath)
    {
        createGraphicsPipeline(vertFilepath, fragFilepath);
    }

    std::vector<char> VE_Pipeline::readFile(const std::string& filepath)
    {
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filepath);
        }
        size_t filesize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(filesize);

        file.seekg(0);
        file.read(buffer.data(), filesize);

        file.close();
        return buffer;
    }

    void VE_Pipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath)
    {
        auto vertCode = readFile(vertFilepath);
        auto fragCode = readFile(fragFilepath);

        std::cout << vertCode.size() << std::endl;
        std::cout << fragCode.size() << std::endl;
    }

}