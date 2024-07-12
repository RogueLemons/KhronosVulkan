#include "FirstApp.h"
#include "SimpleRenderSystem.h"
#include "LVE_Camera.h"
#include "KeyboardMovementController.h"
#include "LVE_Buffer.h"
#include "PointLightSystem.h"
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
		_globalPool = LVE_DescriptorPool::Builder(_lveDevice)
			.setMaxSets(LVE_SwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LVE_SwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

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

		auto globalSetLayout = LVE_DescriptorSetLayout::Builder(_lveDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(LVE_SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			LVE_DescriptorWriter(*globalSetLayout, *_globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem(_lveDevice, _lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
		PointLightSystem pointLightSystem(_lveDevice, _lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
        LVE_Camera camera{};

        auto viewerObject = LVE_GameObject::createGameObject();
		viewerObject._transform.translation.z = -2.5f;
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
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);
			
			if (auto commandBuffer = _lveRenderer.beginFrame()) {
				int frameIndex = _lveRenderer.getFrameIndex();
				FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex], _gameObjects };

				// Update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				pointLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// Render
				_lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);
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
		flatVase._transform.translation = { -0.5f, 0.5f, 0.0f };
		flatVase._transform.scale = { 3.0f, 1.5f, 3.0f };
		_gameObjects.emplace(flatVase.getId(), std::move(flatVase));

		lveModel = LVE_Model::createModelFromFile(_lveDevice, "models/smooth_vase.obj");
		auto smoothVase = LVE_GameObject::createGameObject();
		smoothVase._model = lveModel;
		smoothVase._transform.translation = { 0.5f, 0.5f, 0.0f };
		smoothVase._transform.scale = { 3.0f, 1.5f, 3.0f };
		_gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

		lveModel = LVE_Model::createModelFromFile(_lveDevice, "models/quad.obj");
		auto floor = LVE_GameObject::createGameObject();
		floor._model = lveModel;
		floor._transform.translation = { 0.0f, 0.5f, 0.0f };
		floor._transform.scale = { 3.0f, 1.0f, 3.0f };
		_gameObjects.emplace(floor.getId(), std::move(floor));

		{	// using pointLight again is invalid
			auto pointLight = LVE_GameObject::creatPointLight(0.2f);
			_gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}


	}

}