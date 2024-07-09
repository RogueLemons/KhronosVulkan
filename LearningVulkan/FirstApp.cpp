#include "FirstApp.h"
#include "SimpleRenderSystem.h"
#include "LVE_Camera.h"
#include "KeyboardMovementController.h"
#include <stdexcept>
#include <array>
#include <cassert>
#include <chrono>

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
        LVE_Camera camera{};

        auto viewerObject = LVE_GameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!_lveWindow.shouldClose()) {
			glfwPollEvents();
            
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(_lveWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject._transform.translation, viewerObject._transform.rotation);

            float aspect = _lveRenderer.getAspectRatio();
            // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);
			
			if (auto commandBuffer = _lveRenderer.beginFrame()) {
				_lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, _gameObjects, camera);
				_lveRenderer.endSwapChainRenderPass(commandBuffer);
				_lveRenderer.endFrame();
			}

		}

		vkDeviceWaitIdle(_lveDevice.device());
	}

	void FirstApp::loadGameObjects()
	{
        std::shared_ptr<LVE_Model> lveModel = LVE_Model::createModelFromFile(_lveDevice, "models/smooth_vase.obj");

        auto cube = LVE_GameObject::createGameObject();
        cube._model = lveModel;
        cube._transform.translation = { 0.0f, 0.0f, 2.5f };
		cube._transform.scale = glm::vec3{ 3.0f };

        _gameObjects.push_back(std::move(cube));
	}

}