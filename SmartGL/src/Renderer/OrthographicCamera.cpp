#include "precompiled.h"

#include "Renderer/OrthographicCamera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace SmartGL
{
    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
        : Camera(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_Settings({left, right, bottom, top})
    {
        ComputeViewMatrix();
    }

    void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
    {
        m_Settings = {left, right, bottom, top};
        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        m_ProjectionMatrixInverse = glm::inverse(m_ProjectionMatrix);
        ComputeViewMatrix();
    }

    void OrthographicCamera::ComputeViewMatrix()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

        m_ViewMatrixInverse = transform;
        m_ViewMatrix = glm::inverse(m_ViewMatrixInverse);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}
