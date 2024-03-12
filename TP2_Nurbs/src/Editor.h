#pragma once

#include "SmartGL.h"
#include "Renderer.h"

using namespace SmartGL;

// This class is a layer that inherits from SmartGL::Core::Layer
// It is used to create a layer that will be pushed into the application
// layers have access to the event system and can be updated and rendered

namespace TP2_Nurbs
{
    class Editor : public Core::Layer
    {
    public:
        Editor();
        ~Editor();

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(Timestep timestep) override;
        void OnEvent(Events::Event &event) override;
        void OnUIRender() override;

    private:
        bool OnMouseMoved(const Events::MouseMovedEvent &e);
        bool OnMouseButtonPressed(const Events::MouseButtonPressedEvent &e);
        bool OnKeyPressed(const Events::KeyPressedEvent &e);
        bool OnWindowResize(const Events::WindowResizeEvent &e);
        void DragSelectedPoint();


    private:
        BSplineSurface m_Surface;
        Shared<PerspectiveCamera> m_Camera;
        Shared<ArcBallCameraController> m_CameraController;
    };
}
