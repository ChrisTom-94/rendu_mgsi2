#pragma once

#include "Renderer/Camera.h"

#include "glm/gtc/quaternion.hpp"

namespace SmartGL
{

    struct PerspectiveCameraSettings
    {
        float Fov;
        float AspectRatio;
        float NearClip;
        float FarClip;
    };

    class PerspectiveCamera : public Camera
    {

    public:
        PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip);
        PerspectiveCamera(PerspectiveCameraSettings settings);

        void SetProjection(float fov, float aspectRatio, float nearClip, float farClip);

        glm::vec3 ScreenToWorld(const glm::vec2 &screenPosition, const glm::vec2 &screenSize);

        inline const glm::vec3 &GetPosition() const { return m_Position; }
        inline const glm::quat &GetOrientation() const { return m_Orientation; }
        inline const glm::vec3 &GetRotation() const { return m_Rotation; }

        inline void SetPosition(const glm::vec3 &position)
        {
            m_Position = position;
        }
        inline void SetRotation(const glm::quat &rotation)
        {
            m_Orientation = rotation;
            m_Rotation = glm::eulerAngles(m_Orientation);
        }
        inline void SetRotation(const glm::vec3 &rotation)
        {
            m_Orientation = glm::quat(rotation);
            m_Rotation = rotation;
        }

        inline float const GetFov() { return m_Settings.Fov; }
        inline float const GetAspectRatio() { return m_Settings.AspectRatio; }
        inline float const GetNearClip() { return m_Settings.NearClip; }
        inline float const GetFarClip() { return m_Settings.FarClip; }

        inline void SetFov(float fov)
        {
            m_Settings.Fov = fov;
            SetProjection(m_Settings.Fov, m_Settings.AspectRatio, m_Settings.NearClip, m_Settings.FarClip);
        }

        inline void SetAspectRatio(float aspectRatio)
        {
            m_Settings.AspectRatio = aspectRatio;
            SetProjection(m_Settings.Fov, m_Settings.AspectRatio, m_Settings.NearClip, m_Settings.FarClip);
        }

        inline void SetNearClip(float nearClip)
        {
            m_Settings.NearClip = nearClip;
            SetProjection(m_Settings.Fov, m_Settings.AspectRatio, m_Settings.NearClip, m_Settings.FarClip);
        }

        inline void SetFarClip(float farClip)
        {
            m_Settings.FarClip = farClip;
            SetProjection(m_Settings.Fov, m_Settings.AspectRatio, m_Settings.NearClip, m_Settings.FarClip);
        }

        void ComputeViewMatrix();
        void LookAt(const glm::vec3 &target, const glm::vec3 &up = glm::vec3(0.0f, 1.0f, 0.0f));

    private:
        PerspectiveCameraSettings m_Settings;

        glm::quat m_Orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 m_Rotation = glm::vec3(0.0f);
        glm::vec3 m_Position = glm::vec3(0.0f);
    };
}
