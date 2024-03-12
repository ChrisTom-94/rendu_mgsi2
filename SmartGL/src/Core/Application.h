#pragma once

#include "Core/TypesDefinitions.h"
#include "Core/Window.h"
#include "Core/Layer.h"
#include "Core/LayerStack.h"

#include "Events/Event.h"

#include "UI/UILayer.h"

int main(int argc, char **argv);

namespace SmartGL
{
    namespace Core
    {

        struct ApplicationSpecification
        {
            std::string Name = "SmartGL Application";
            std::string WorkingDirectory;
            uint32_t Width = 1280;
            uint32_t Height = 720;
            bool VSync = true;
        };

        class Application
        {
        public:
            Application(ApplicationSpecification specification);
            virtual ~Application();

            void Run();

            void PushLayer(Layer *layer);
            void PushOverlay(Layer *overlay);

            void OnEvent(Events::Event &e);

            inline static Application &Get() { return *s_Instance; }
            inline Window &GetWindow() { return *m_Window; }
            inline ApplicationSpecification &GetSpecification() { return m_Specification; }

        private:
            bool OnWindowClose(Events::WindowCloseEvent &e);
            bool OnWindowResize(Events::WindowResizeEvent &e);

        private:
            ApplicationSpecification m_Specification;
            Unique<Window> m_Window;
            UI::UILayer *m_UILayer;
            LayerStack m_LayerStack;

            bool m_Minimized = false;
            bool m_Running = true;
            float m_LastFrameTime = 0.0f;

        private:
            static Application *s_Instance;
            friend int ::main(int argc, char **argv);
        };

        Application *CreateApplication();
    }
}
