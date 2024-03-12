#pragma once

#include "Core/Core.h"
#include "Core/TypesDefinitions.h"

#include "Renderer/PerspectiveCamera.h"

#include "Events/Event.h"
#include "Events/MouseEvent.h"

#define WORLD_UP glm::vec3(0.0f, 1.0f, 0.0f)
#define WORLD_RIGHT glm::vec3(1.0f, 0.0f, 0.0f)
#define WORLD_FORWARD glm::vec3(0.0f, 0.0f, 1.0f)

namespace SmartGL
{

    class OrbitCameraController
    {

    public:
        enum class OrbitCameraState
        {
            Static = BIT(0),
            Pan = BIT(1),
            Orbit = BIT(2),
            Zoom = BIT(3),

            All = Static | Pan | Orbit | Zoom,
        };

        friend uint32_t operator|(OrbitCameraState a, OrbitCameraState b)
        {
            return static_cast<uint32_t>(a) | static_cast<uint32_t>(b);
        }

        friend uint32_t operator&(OrbitCameraState a, OrbitCameraState b)
        {
            return static_cast<uint32_t>(a) & static_cast<uint32_t>(b);
        }

        friend uint32_t operator~(OrbitCameraState a)
        {
            return ~static_cast<uint32_t>(a);
        }

    public:
        OrbitCameraController(Shared<PerspectiveCamera> &camera);

        void OnUpdate(float deltaTime);

        inline const Shared<PerspectiveCamera> &GetCamera() const { return m_Camera; }
        inline Shared<PerspectiveCamera> &GetCamera() { return m_Camera; }

        inline void SetPosition(const glm::vec3 &position) { m_Camera->SetPosition(position); }
        inline void SetRotation(const glm::vec3 &rotation) { m_Camera->SetRotation(rotation); }

        inline void SetCameraTranslationSpeed(float speed) { m_CameraTranslationSpeed = speed; }
        inline void SetCameraRotationSpeed(float speed) { m_CameraRotationSpeed = speed; }
        inline void SetCameraZoomSpeed(float speed) { m_CameraZoomSpeed = speed; }

        inline void SetTarget(const glm::vec3 &target) { m_Target = target; }
        inline void SetUp(const glm::vec3 &up) { m_Up = up; }

        bool OnEvent(Events::Event &event);

        void SetAvailableState(OrbitCameraState state);
        void UnsetAvailableState(OrbitCameraState state);

        void UpdateView();

    private:
        bool OnMouseMoved(Events::MouseMovedEvent &event);
        bool OnMouseScrolled(Events::MouseScrolledEvent &event);

        void RotateAzimuth(float angle);
        void RotatePolar(float angle);
        void MoveHorizontal(float distance);
        void MoveVertical(float distance);
        void Zoom(float deltaY);

        glm::vec3 GetNormalizedViewVector() const;

    private:
        Shared<PerspectiveCamera> m_Camera;

        float m_CameraTranslationSpeed = 1.0f;
        float m_CameraRotationSpeed = 2.0f;
        float m_CameraZoomSpeed = 1.0f;

        glm::vec3 m_Target = glm::vec3(0.0f);

        glm::vec2 m_LastMousePosition = glm::vec2(0.0f);

        float m_AzimuthAngle = 0.0f;
        float m_PolarAngle = 0.0f;

        float m_Radius = 2.0f; // distance from target
        float m_MinRadius = 2.0f;
        float m_MaxRadius = 10.0f;

        glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

        OrbitCameraState m_State = OrbitCameraState::Static;
        OrbitCameraState m_AvailableStates = OrbitCameraState::All;
    };
}
