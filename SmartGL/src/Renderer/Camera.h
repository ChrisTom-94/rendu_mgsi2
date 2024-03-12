#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace SmartGL
{
	class Camera
	{
	public:
		Camera() = default;

		Camera(const glm::mat4 &projectionMatrix) : m_ProjectionMatrix(projectionMatrix) {}

		virtual ~Camera() = default;

		const glm::mat4 &GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4 &GetProjectionMatrixInverse() const { return m_ProjectionMatrixInverse; }
		const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4 &GetViewMatrixInverse() const { return m_ViewMatrixInverse; }
		const glm::mat4 &GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	protected:
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ProjectionMatrixInverse = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrixInverse = glm::mat4(1.0f);
		glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);
	};
}
