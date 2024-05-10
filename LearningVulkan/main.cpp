
#include <iostream>
#include <stdexcept>

int main() {
    try {

        // Run main

    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}