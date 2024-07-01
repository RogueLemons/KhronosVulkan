#pragma once
#include "LVE_Model.h"
#include <memory>

#include <glm/gtc/matrix_transform.hpp>

namespace LVE {

	struct TransformComponent {
		glm::vec3 translation{}; // (position offset)
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation{};

		// Matrix corresponds to translation * Ry * Rx * Rz * scale transformation.
		// Rotation convention uses Tait-Bryan with axis order Y(1), X(2), Z(3).
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4() {
			// auto transform = glm::translate(glm::mat4{ 1.0f }, translation);
			// transform = glm::rotate(transform, rotation.y, { 0.0f, 1.0f, 0.0f });
			// transform = glm::rotate(transform, rotation.x, { 1.0f, 0.0f, 0.0f });
			// transform = glm::rotate(transform, rotation.z, { 0.0f, 0.0f, 1.0f });
			// transform = glm::scale(transform, scale);
			// return transform;

			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);
			return glm::mat4{
				{
					scale.x * (c1 * c3 + s1 * s2 * s3),
					scale.x * (c2 * s3),
					scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					scale.y * (c3 * s1 * s2 - c1 * s3),
					scale.y * (c2 * c3),
					scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					scale.z * (c2 * s1),
					scale.z * (-s2),
					scale.z * (c1 * c2),
					0.0f,
				},
				{translation.x, translation.y, translation.z, 1.0f} };
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
		TransformComponent _transform{};

	private:
		LVE_GameObject(id_t objId) : _id(objId) {  };

		id_t _id;
	};

}


