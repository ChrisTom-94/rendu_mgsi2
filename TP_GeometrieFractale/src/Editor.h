#pragma once

#include "SmartGL.h"

using namespace SmartGL;

namespace TP_GeometrieFractale
{
    class Editor : public Core::Layer
    {
    public:
        Editor();
        ~Editor();

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(Timestep deltaTime) override;
        void OnUIRender() override;
        void OnEvent(Events::Event &event) override;

    private:
        bool OnWindowResize(Events::WindowResizeEvent &event);

    private:
        Shared<PerspectiveCamera> m_Camera;
        Shared<ArcBallCameraController> m_CameraController;
    };
}
