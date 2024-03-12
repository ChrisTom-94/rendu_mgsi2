#pragma once

#include "SmartGL.h"
#include "Renderer.h"

using namespace SmartGL;

// This class is a layer that inherits from SmartGL::Core::Layer
// It is used to create a layer that will be pushed into the application
// layers have access to the event system and can be updated and rendered

namespace TP1_Nurbs
{
    class Editor : public Core::Layer
    {
    public:
        Editor();
        ~Editor();

        void OnAttach() override;
        void OnDetach() override {};
        void OnUpdate(Timestep timestep) override;
        void OnEvent(Events::Event &event) override;
        void OnUIRender() override;

    private:
        void AddControlPoint();
        void DeleteControlPoint(const uint8_t &id);
        void DragSelectedPoint();
        void CancelDragging();

        bool OnMouseMoved(const Events::MouseMovedEvent &e);
        bool OnKeyPressed(const Events::KeyPressedEvent &e);
        bool OnMouseButtonPressed(const Events::MouseButtonPressedEvent &e);
        bool OnWindowResize(const Events::WindowResizeEvent &e);

    private:
        BSplineCurve m_Spline;
        Shared<PerspectiveCamera> m_Camera;
        Shared<ArcBallCameraController> m_CameraController;
    };
}
