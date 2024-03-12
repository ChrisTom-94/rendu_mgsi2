#include "precompiled.h"

#include "Renderer/CameraControllers/OrbitCameraController.h"

#include "Core/Input.h"

namespace SmartGL
{
    OrbitCameraController::OrbitCameraController(Shared<PerspectiveCamera> &camera)
        : m_Camera(camera),
          m_Target(0.0f),
          m_Radius(10.0f),
          m_MinRadius(1.0f),
          m_AzimuthAngle(0.0f),
          m_PolarAngle(0.0f),
          m_Up(WORLD_UP),
          m_AvailableStates(OrbitCameraState::All)
    {
        m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, m_Radius));
    }

    void OrbitCameraController::OnUpdate(float ts)
    {
    }

    bool OrbitCameraController::OnEvent(Events::Event &event)
    {
        Events::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Events::MouseMovedEvent>(BIND_EVENT_FN(OrbitCameraController::OnMouseMoved));
        dispatcher.Dispatch<Events::MouseScrolledEvent>(BIND_EVENT_FN(OrbitCameraController::OnMouseScrolled));

        return false;
    }

    bool OrbitCameraController::OnMouseMoved(Events::MouseMovedEvent &event)
    {
        bool isPanning = Input::IsMouseButtonPressed(Events::ButtonRight);
        bool isOrbiting = Input::IsMouseButtonPressed(Events::ButtonLeft);
        bool isStatic = !isPanning && !isOrbiting;

        if (isStatic)
            return false;

        if (isPanning)
        {
            glm::vec2 mousePos = Input::GetMousePosition();
            float deltaX = mousePos.x - m_LastMousePosition.x;
            float deltaY = mousePos.y - m_LastMousePosition.y;
            m_LastMousePosition = mousePos;

            MoveHorizontal(deltaX);
            MoveVertical(deltaY);
        }

        if (isOrbiting)
        {
            glm::vec2 mousePos = Input::GetMousePosition();
            float deltaX = mousePos.x - m_LastMousePosition.x;
            float deltaY = mousePos.y - m_LastMousePosition.y;
            m_LastMousePosition = mousePos;

            RotateAzimuth(deltaX);
            RotatePolar(deltaY);
        }

        if (isPanning || isOrbiting)
            UpdateView();

        m_LastMousePosition = Input::GetMousePosition();

        return false;
    }

    bool OrbitCameraController::OnMouseScrolled(Events::MouseScrolledEvent &event)
    {
        float deltaY = event.GetYOffset();
        Zoom(deltaY);
        UpdateView();

        return false;
    }

    void OrbitCameraController::SetAvailableState(OrbitCameraState state)
    {
        m_AvailableStates = static_cast<OrbitCameraState>(m_AvailableStates | state);
    }

    void OrbitCameraController::UnsetAvailableState(OrbitCameraState state)
    {
        m_AvailableStates = static_cast<OrbitCameraState>(m_AvailableStates & static_cast<OrbitCameraState>(~state));
    }

    void OrbitCameraController::RotateAzimuth(float angle)
    {
        float radians = glm::radians(angle);
        m_AzimuthAngle += radians;

        const auto fullCircle = 2.0f * glm::pi<float>();
        m_AzimuthAngle = fmodf(m_AzimuthAngle, fullCircle);
        if (m_AzimuthAngle < 0.0f)
            m_AzimuthAngle = fullCircle + m_AzimuthAngle;
    }

    void OrbitCameraController::RotatePolar(float angle)
    {
        float radians = glm::radians(angle);
        m_PolarAngle += radians;
        m_PolarAngle = glm::clamp(m_PolarAngle, -glm::half_pi<float>(), glm::half_pi<float>());
    }

    void OrbitCameraController::Zoom(float deltaY)
    {
        m_Radius -= deltaY;
        if (m_Radius < m_MinRadius)
            m_Radius = m_MinRadius;
    }

    void OrbitCameraController::UpdateView()
    {
        const auto sineAzimuth = sin(m_AzimuthAngle);
        const auto cosineAzimuth = cos(m_AzimuthAngle);
        const auto sinePolar = sin(m_PolarAngle);
        const auto cosinePolar = cos(m_PolarAngle);

        const auto x = m_Target.x + m_Radius * cosinePolar * cosineAzimuth;
        const auto y = m_Target.y + m_Radius * sinePolar;
        const auto z = m_Target.z + m_Radius * cosinePolar * sineAzimuth;

        m_Camera->SetPosition(glm::vec3(x, y, z));
    }

    void OrbitCameraController::MoveHorizontal(float distance)
    {
        glm::vec3 viewVector = GetNormalizedViewVector();
        glm::vec3 strafeVector = glm::normalize(glm::cross(viewVector, m_Up));
        m_Target += strafeVector * distance;
    }

    void OrbitCameraController::MoveVertical(float distance)
    {
        m_Target += m_Up * distance;
    }

    glm::vec3 OrbitCameraController::GetNormalizedViewVector() const
    {
        return glm::normalize(m_Target - m_Camera->GetPosition());
    }

}
