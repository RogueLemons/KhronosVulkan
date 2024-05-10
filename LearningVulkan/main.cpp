#include <iostream>
#include <stdexcept>

#include "FirstApp.h"

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