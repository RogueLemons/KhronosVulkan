#include "LVE_Camera.h"
#include <cassert>
#include <limits>

namespace LVE {

	void LVE_Camera::setOrthographicProjection(
		float left, float right, float top, float bottom, float near, float far) {
		_projectionMatrix = glm::mat4{ 1.0f };
		_projectionMatrix[0][0] = 2.0f / (right - left);
		_projectionMatrix[1][1] = 2.0f / (bottom - top);
		_projectionMatrix[2][2] = 1.0f / (far - near);
		_projectionMatrix[3][0] = -(right + left) / (right - left);
		_projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
		_projectionMatrix[3][2] = -near / (far - near);
	}

	void LVE_Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
		assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);

		const float tanHalfFovy = tan(fovy / 2.0f);
		_projectionMatrix = glm::mat4{ 0.0f };
		_projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
		_projectionMatrix[1][1] = 1.f / (tanHalfFovy);
		_projectionMatrix[2][2] = far / (far - near);
		_projectionMatrix[2][3] = 1.f;
		_projectionMatrix[3][2] = -(far * near) / (far - near);
	}

}

