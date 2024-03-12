#include "precompiled.h"

#include "Renderer/PerspectiveCamera.h"

namespace SmartGL
{

    PerspectiveCamera::PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : Camera(glm::perspective(fov, aspectRatio, nearClip, farClip)), m_Settings({fov, aspectRatio, nearClip, farClip})
    {
        ComputeViewMatrix();
    }

    PerspectiveCamera::PerspectiveCamera(PerspectiveCameraSettings settings)
        : Camera(glm::perspective(settings.Fov, settings.AspectRatio, settings.NearClip, settings.FarClip)),
          m_Settings(settings)
    {
        ComputeViewMatrix();
    }

    void PerspectiveCamera::SetProjection(float fov, float aspectRatio, float nearClip, float farClip)
    {
        m_Settings = {fov, aspectRatio, nearClip, farClip};
        m_ProjectionMatrix = glm::perspective(fov, aspectRatio, nearClip, farClip);
        m_ProjectionMatrixInverse = glm::inverse(m_ProjectionMatrix);
        ComputeViewMatrix();
    }

    void PerspectiveCamera::ComputeViewMatrix()
    {
        glm::mat4 rotate = glm::mat4_cast(m_Orientation);
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), m_Position);

        m_ViewMatrix = translate * rotate;
        m_ViewMatrixInverse = glm::inverse(m_ViewMatrix);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void PerspectiveCamera::LookAt(const glm::vec3 &target, const glm::vec3 &up)
    {
        glm::mat4 lookAt = glm::lookAt(m_Position, target, up);

        m_ViewMatrix = lookAt * glm::mat4_cast(m_Orientation);
        m_ViewMatrixInverse = glm::inverse(m_ViewMatrix);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    glm::vec3 PerspectiveCamera::ScreenToWorld(const glm::vec2 &screenPosition, const glm::vec2 &screenSize)
    {
        glm::vec4 viewport = {0.0f, 0.0f, screenSize.x, screenSize.y};
        glm::vec3 window = glm::vec3(screenPosition, 0.0f);
        window.y = screenSize.y - window.y;
        return glm::unProject(window, m_ViewMatrix, m_ProjectionMatrix, viewport);
    }
}
