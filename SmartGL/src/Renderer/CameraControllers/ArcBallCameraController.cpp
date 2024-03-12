#include "precompiled.h"

#include "Renderer/CameraControllers/ArcBallCameraController.h"

#include "Core/Application.h"
#include "Core/Input.h"

#include "glm/gtx/quaternion.hpp"

namespace SmartGL
{
    ArcBallCameraController::ArcBallCameraController(Shared<PerspectiveCamera> &camera, glm::vec3 center, float radius)
        : m_Camera(camera),
          m_Center(center),
          m_Radius(radius),
          m_PolarAngle(0.0f),
          m_AzimuthAngle(0.0f),
          m_Settings(ArcBallCameraSettings())
    {
        m_Camera->SetPosition(glm::vec3(center.x, center.y, center.z - radius));
        m_Camera->SetRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
        m_Camera->LookAt(center, glm::vec3(0.0f, 1.0f, 0.0f));

        m_LastMousePosition = {0.0f, 0.0f};
    }

    ArcBallCameraController::~ArcBallCameraController()
    {
    }

    void ArcBallCameraController::OnUpdate(float deltaTime)
    {
    }

    void ArcBallCameraController::OnEvent(Events::Event &event)
    {
        Events::EventDispatcher dispatcher(event);
        dispatcher.Dispatch<Events::MouseMovedEvent>(BIND_EVENT_FN(ArcBallCameraController::OnMouseMoved));
        dispatcher.Dispatch<Events::MouseScrolledEvent>(BIND_EVENT_FN(ArcBallCameraController::OnMouseScrolled));
    }

    bool ArcBallCameraController::OnMouseMoved(Events::MouseMovedEvent &event)
    {
        glm::vec2 mousePosition = Input::GetMousePosition();

        if (!Input::IsMouseButtonPressed(m_Settings.Trigger))
        {
            m_LastMousePosition = mousePosition;
            return false;
        }

        static bool firstMouseInput = false;
        if (!firstMouseInput)
        {
            m_LastMousePosition = mousePosition;
            firstMouseInput = true;
        }

        int width = Core::Application::Get().GetWindow().GetWidth();
        int height = Core::Application::Get().GetWindow().GetHeight();

        if(m_LastMousePosition == glm::vec2(0.0f, 0.0f))
            m_LastMousePosition = mousePosition;

        float deltaX = mousePosition.x - m_LastMousePosition.x;
        float deltaY = mousePosition.y - m_LastMousePosition.y;

        if (deltaX != 0.0f)
            RotateAzimuth(deltaX);

        if (deltaY != 0.0f)
            RotatePolar(deltaY);

        UpdateCamera();

        m_LastMousePosition = mousePosition;

        return false;
    }

    bool ArcBallCameraController::OnMouseScrolled(Events::MouseScrolledEvent &event)
    {
        float delta = event.GetYOffset();
        Zoom(delta);

        return false;
    }

    void ArcBallCameraController::Zoom(float delta)
    {
        m_Radius -= delta * m_Settings.ZoomSpeed;
        m_Radius = glm::clamp(m_Radius, m_Settings.MinRadius, m_Settings.MaxRadius);
        UpdateCamera();
    }

    void ArcBallCameraController::RotatePolar(float angle)
    {
        m_PolarAngle += angle * m_Settings.RotationSpeed;
        m_PolarAngle = glm::mod(m_PolarAngle, 360.0f);
        m_PolarAngle = glm::clamp(m_PolarAngle, m_Settings.MinPolarAngle, m_Settings.MaxPolarAngle);
    }

    void ArcBallCameraController::RotateAzimuth(float angle)
    {
        m_AzimuthAngle += angle * m_Settings.RotationSpeed;
        m_AzimuthAngle = glm::mod(m_AzimuthAngle, 360.0f);
        m_AzimuthAngle = glm::clamp(m_AzimuthAngle, m_Settings.MinAzimuthAngle, m_Settings.MaxAzimuthAngle);
    }

    void ArcBallCameraController::UpdateCamera()
    {
        glm::quat rotation = glm::angleAxis(glm::radians(-m_PolarAngle), GetRight()) *
                             glm::angleAxis(glm::radians(m_AzimuthAngle), GetUp());


        m_Camera->SetRotation(rotation);
        m_Camera->SetPosition(m_Center + m_Radius * GetForward());
        m_Camera->LookAt(m_Center, GetUp());
    }

    glm::vec3 const ArcBallCameraController::GetRight() const
    {
        return glm::normalize(glm::cross(GetForward(), glm::vec3(0.0f, 1.0f, 0.0f)));
    }

    glm::vec3 const ArcBallCameraController::GetUp() const
    {
        return glm::normalize(glm::cross(GetRight(), GetForward()));
    }

    glm::vec3 const ArcBallCameraController::GetForward() const
    {
        return glm::normalize(m_Camera->GetPosition() - m_Center);
    }
}
