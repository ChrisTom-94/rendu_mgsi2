#pragma once

#include "Renderer/Camera.h"

namespace SmartGL
{

    struct OrthographicCameraSettings
    {
        float Left;
        float Right;
        float Bottom;
        float Top;
    };

    class OrthographicCamera : public Camera
    {

    public:
        OrthographicCamera(float left, float right, float bottom, float top);

        void SetProjection(float left, float right, float bottom, float top);

        const glm::vec3 &GetPosition() const { return m_Position; }
        void SetPosition(const glm::vec3 &position)
        {
            m_Position = position;
            ComputeViewMatrix();
        }

        float GetRotation() const { return m_Rotation; }
        void SetRotation(float rotation)
        {
            m_Rotation = rotation;
            ComputeViewMatrix();
        }

    private:
        void ComputeViewMatrix();

    private:
        OrthographicCameraSettings m_Settings;

        glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
        float m_Rotation = 0.0f;
    };
}
