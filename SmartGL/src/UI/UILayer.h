#pragma once

#include "Core/Layer.h"

#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvent.h"

namespace SmartGL
{
    namespace UI
    {
        class UILayer : public Core::Layer
        {
        public:
            UILayer();
            ~UILayer();

            void OnAttach() override;
            void OnDetach() override;
            void OnEvent(Events::Event &e);

            void OnUIRenderStart();
            void OnUIRender() override;
            void OnUIRenderEnd();

        private:
            bool m_BlockEvents = true;
        };
    }
}
