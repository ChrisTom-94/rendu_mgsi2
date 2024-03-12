#include "precompiled.h"

#include "Renderer/CameraControllers/OrthographicCameraController.h"

#include "Core/Input.h"

namespace SmartGL
{

    OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
        : m_AspectRatio(aspectRatio), 
          m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), 
          m_Rotation(rotation)
    {
    }

    void OrthographicCameraController::OnUpdate(float deltaTime)
    {
        if (Input::IsKeyPressed(Events::Key::A))
            m_CameraPosition.x -= m_CameraTranslationSpeed * deltaTime;
        else if (Input::IsKeyPressed(Events::Key::D))
            m_CameraPosition.x += m_CameraTranslationSpeed * deltaTime;

        if (Input::IsKeyPressed(Events::Key::W))
            m_CameraPosition.y += m_CameraTranslationSpeed * deltaTime;
        else if (Input::IsKeyPressed(Events::Key::S))
            m_CameraPosition.y -= m_CameraTranslationSpeed * deltaTime;

        if (m_Rotation)
        {
            if (Input::IsKeyPressed(Events::Key::Q))
                m_CameraRotation += m_CameraRotationSpeed * deltaTime;
            else if (Input::IsKeyPressed(Events::Key::E))
                m_CameraRotation -= m_CameraRotationSpeed * deltaTime;

            m_Camera.SetRotation(m_CameraRotation);
        }

        m_Camera.SetPosition(m_CameraPosition);

        m_CameraTranslationSpeed = m_ZoomLevel;

    }

    void OrthographicCameraController::OnEvent(Events::Event &e)
    {
        Events::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<Events::MouseScrolledEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
        dispatcher.Dispatch<Events::WindowResizeEvent>(BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
    }

    bool OrthographicCameraController::OnMouseScrolled(Events::MouseScrolledEvent &e)
    {
        m_ZoomLevel -= e.GetYOffset() * 0.25f;
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        return false;
    }

    bool OrthographicCameraController::OnWindowResized(Events::WindowResizeEvent &e)
    {
        m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        return false;
    }

}