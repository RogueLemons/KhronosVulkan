#include "FirstApp.h"

void FirstApp::run()
{
	while (!_veWindow.shouldClose()) {
		glfwPollEvents();
	}
}
