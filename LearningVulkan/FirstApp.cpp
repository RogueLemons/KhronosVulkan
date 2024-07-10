#include "FirstApp.h"
#include "SimpleRenderSystem.h"
#include "LVE_Camera.h"
#include "KeyboardMovementController.h"
#include "LVE_Buffer.h"
#include <stdexcept>
#include <array>
#include <cassert>
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace LVE {

	struct GlobalUbo {
		glm::mat4 projectionView{ 1.0f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.0f, -3.0f, -1.0f });
	};
	
	FirstApp::FirstApp()
	{
		loadGameObjects();
	}

	FirstApp::~FirstApp() {  }

	void FirstApp::run()
	{
		std::vector<std::unique_ptr<LVE_Buffer>> uboBuffers(LVE_SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<LVE_Buffer>(
				_lveDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			uboBuffers[i]->map();
		}

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
				int frameIndex = _lveRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera
				};

				// Update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// Render
				_lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo, _gameObjects);
				_lveRenderer.endSwapChainRenderPass(commandBuffer);
				_lveRenderer.endFrame();
			}

		}

		vkDeviceWaitIdle(_lveDevice.device());
	}

	void FirstApp::loadGameObjects()
	{
		std::shared_ptr<LVE_Model> lveModel = LVE_Model::createModelFromFile(_lveDevice, "models/flat_vase.obj");
		auto flatVase = LVE_GameObject::createGameObject();
		flatVase._model = lveModel;
		flatVase._transform.translation = { -0.5f, 0.5f, 2.5f };
		flatVase._transform.scale = { 3.0f, 1.5f, 3.0f };
		_gameObjects.push_back(std::move(flatVase));

		lveModel = LVE_Model::createModelFromFile(_lveDevice, "models/smooth_vase.obj");
		auto smoothVase = LVE_GameObject::createGameObject();
		smoothVase._model = lveModel;
		smoothVase._transform.translation = { 0.5f, 0.5f, 2.5f };
		smoothVase._transform.scale = { 3.0f, 1.5f, 3.0f };
		_gameObjects.push_back(std::move(smoothVase));
	}

}