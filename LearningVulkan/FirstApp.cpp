#include "FirstApp.h"
#include "SimpleRenderSystem.h"
#include <stdexcept>
#include <array>
#include <cassert>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace LVE {
	
	FirstApp::FirstApp()
	{
		loadGameObjects();
	}

	FirstApp::~FirstApp() {  }

	void FirstApp::run()
	{
		SimpleRenderSystem simpleRenderSystem(_lveDevice, _lveRenderer.getSwapChainRenderPass());

		while (!_lveWindow.shouldClose()) {
			glfwPollEvents();
			
			if (auto commandBuffer = _lveRenderer.beginFrame()) {
				_lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, _gameObjects);
				_lveRenderer.endSwapChainRenderPass(commandBuffer);
				_lveRenderer.endFrame();
			}

		}

		vkDeviceWaitIdle(_lveDevice.device());
	}

	void FirstApp::loadGameObjects()
	{
		std::vector<LVE_Model::Vertex> vertices{
			{ { 0.0f, -0.5f },	{ 1.0f, 0.0f, 0.0f } },
			{ { 0.5f, 0.5f },	{ 0.0f, 1.0f, 0.0f } },
			{ { -0.5f, 0.5f },	{ 0.0f, 0.0f, 1.0f } },
		};
		auto lveModel = std::make_shared<LVE_Model>(_lveDevice, vertices);

		auto triangle = LVE_GameObject::createGameObject();
		triangle._model = lveModel;
		triangle._color = { 0.1f, 0.8f, 0.1f };
		triangle._transform2d.translation.x = 0.2f;
		triangle._transform2d.scale = { 2.0f, 0.5f };
		triangle._transform2d.rotation = 0.25f * glm::two_pi<float>();
		
		_gameObjects.push_back(std::move(triangle));
	}

}