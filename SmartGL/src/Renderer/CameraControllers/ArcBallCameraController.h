#pragma once

#include "Core/TypesDefinitions.h"

#include "Renderer/PerspectiveCamera.h"

#include "Events/Event.h"
#include "Events/MouseEvent.h"

namespace SmartGL
{

    struct ArcBallCameraSettings
    {
        float RotationSpeed = 0.3f;
        float ZoomSpeed = 0.3f;

        float MaxPolarAngle = 360.0f;
        float MinPolarAngle = 0.0f;

        float MaxAzimuthAngle = 360.0f;
        float MinAzimuthAngle = 0.0f;

        float MaxRadius = 15.0f;
        float MinRadius = 3.0f;

        bool PolarEnabled = true;
        bool AzimuthEnabled = true;
        bool ZoomEnabled = true;

        Events::MouseCode Trigger = Events::ButtonMiddle;
    };

    class ArcBallCameraController
    {
    public:
        ArcBallCameraController(Shared<PerspectiveCamera> &camera, glm::vec3 center, float radius);
        ~ArcBallCameraController();

        void OnUpdate(float deltaTime);
        void OnEvent(Events::Event &event);

        inline const Shared<PerspectiveCamera> &GetCamera() const { return m_Camera; }

        inline void SetCenter(const glm::vec3 &center) { m_Center = center; }
        inline void SetRadius(float radius) { m_Radius = radius; }

        inline const glm::vec3 &GetCenter() const { return m_Center; }
        inline float GetRadius() const { return m_Radius; }

        inline void SetMaxPolarAngle(float angle) { m_Settings.MaxPolarAngle = angle; }
        inline void SetMinPolarAngle(float angle) { m_Settings.MinPolarAngle = angle; }

        inline void SetMaxAzimuthAngle(float angle) { m_Settings.MaxAzimuthAngle = angle; }
        inline void SetMinAzimuthAngle(float angle) { m_Settings.MinAzimuthAngle = angle; }

        inline void SetPolarEnabled(bool enabled) { m_Settings.PolarEnabled = enabled; }
        inline void SetAzimuthEnabled(bool enabled) { m_Settings.AzimuthEnabled = enabled; }

        inline void SetRotationSpeed(float speed) { m_Settings.RotationSpeed = speed; }

        inline void SetMinRadius(float radius) { m_Settings.MinRadius = radius; }
        inline void SetMaxRadius(float radius) { m_Settings.MaxRadius = radius; }

        inline float GetMaxPolarAngle() const { return m_Settings.MaxPolarAngle; }
        inline float GetMinPolarAngle() const { return m_Settings.MinPolarAngle; }

        inline float GetMaxAzimuthAngle() const { return m_Settings.MaxAzimuthAngle; }
        inline float GetMinAzimuthAngle() const { return m_Settings.MinAzimuthAngle; }

        inline float GetMaxRadius() const { return m_Settings.MaxRadius; }
        inline float GetMinRadius() const { return m_Settings.MinRadius; }

        inline bool IsPolarEnabled() const { return m_Settings.PolarEnabled; }
        inline bool IsAzimuthEnabled() const { return m_Settings.AzimuthEnabled; }
        inline bool IsZoomEnabled() const { return m_Settings.ZoomEnabled; }

        inline Events::MouseCode GetTrigger() const { return m_Settings.Trigger; }
        inline void SetTrigger(Events::MouseCode trigger) { m_Settings.Trigger = trigger; }

        inline float GetRotationSpeed() const { return m_Settings.RotationSpeed; }

        glm::vec3 const GetRight() const;
        glm::vec3 const GetUp() const;
        glm::vec3 const GetForward() const;

        glm::vec3 GetPosition() const { return m_Camera->GetViewMatrixInverse()[3]; }

    private:
        void RotatePolar(float angle);
        void RotateAzimuth(float angle);
        void Zoom(float delta);
        void UpdateCamera();

        bool OnMouseMoved(Events::MouseMovedEvent &event);
        bool OnMouseScrolled(Events::MouseScrolledEvent &event);

    private:
        Shared<PerspectiveCamera> m_Camera;
        ArcBallCameraSettings m_Settings;

        glm::vec3 m_Center;
        glm::vec3 m_Up = {0.0f, 1.0f, 0.0f};
        glm::vec3 m_Right = {1.0f, 0.0f, 0.0f};
        glm::vec3 m_Forward = {0.0f, 0.0f, -1.0f};
        glm::vec2 m_LastMousePosition;

        float m_Radius;
        float m_PolarAngle;
        float m_AzimuthAngle;
    };
}
