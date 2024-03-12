#pragma once

#include "SmartGL.h"
#include "Renderer.h"

using namespace SmartGL;

namespace TP_Deformations
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
        bool OnWindowResize(const Events::WindowResizeEvent &e);


    private:
        Shared<PerspectiveCamera> m_Camera;
        Shared<ArcBallCameraController> m_CameraController;
    };
}
