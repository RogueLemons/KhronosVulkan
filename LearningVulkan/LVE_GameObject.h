#pragma once
#include "LVE_Model.h"
#include <memory>

#include <glm/gtc/matrix_transform.hpp>

namespace LVE {

	struct TransformComponent {
		glm::vec3 translation{}; // (position offset)
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation{};

		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	class LVE_GameObject
	{
	public:
		using id_t = unsigned int;

		static LVE_GameObject createGameObject() {
			static id_t currentId = 0;
			return LVE_GameObject(currentId++);
		}

		LVE_GameObject(const LVE_GameObject&) = delete;
		LVE_GameObject& operator =(const LVE_GameObject&) = delete;
		LVE_GameObject(LVE_GameObject&&) = default;
		LVE_GameObject& operator =(LVE_GameObject&&) = default;

		id_t getId() const { return _id; }

		std::shared_ptr<LVE_Model> _model{};
		glm::vec3 _color{};
		TransformComponent _transform{};

	private:
		LVE_GameObject(id_t objId) : _id(objId) {  };

		id_t _id;
	};

}


