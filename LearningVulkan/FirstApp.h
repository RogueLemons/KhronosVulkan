#pragma once
#include "LVE_Window.h"
#include "LVE_Device.h"
#include "LVE_GameObject.h"
#include "LVE_Renderer.h"
#include <memory>
#include <vector>

namespace LVE {

	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();
		FirstApp(const FirstApp&) = delete;
		FirstApp& operator= (const FirstApp&) = delete;

		void run();

	private:
		void loadGameObjects();

		LVE_Window _lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		LVE_Device _lveDevice{ _lveWindow };
		LVE_Renderer _lveRenderer{ _lveWindow, _lveDevice };

		std::vector<LVE_GameObject> _gameObjects;
	};

}