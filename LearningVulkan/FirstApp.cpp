#include "FirstApp.h"

namespace LVE {

	void FirstApp::run()
	{
		while (!_veWindow.shouldClose()) {
			glfwPollEvents();
		}
	}

}