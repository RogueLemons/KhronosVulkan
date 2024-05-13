#include "FirstApp.h"
#include <iostream>
#include <stdexcept>

// VE - VulkanEngine
// LVE - LittleVulkanEngine
int main() {
    FirstApp app;
    try {

        app.run();

    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}