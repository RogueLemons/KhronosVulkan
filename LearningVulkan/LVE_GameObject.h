#pragma once
#include "LVE_Model.h"
#include <memory>

namespace LVE {

	struct Transform2dComponent {
		glm::vec2 translation{}; // (position offset)
		glm::vec2 scale{ 1.0f, 1.0f };
		float rotation;

		glm::mat2 mat2() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{ {c, s}, {-s, c} };

			glm::mat2 scaleMat{ {scale.x, 0.0f}, {0.0f, scale.y} }; // columns, not rows
			return rotMatrix * scaleMat; 
		}
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
		Transform2dComponent _transform2d{};

	private:
		LVE_GameObject(id_t objId) : _id(objId) {  };

		id_t _id;
	};

}


