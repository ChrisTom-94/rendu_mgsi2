#include "precompiled.h"

#include "Core/Application.h"
#include "Core/Assert.h"
#include "Core/Time.h"

#include "Renderer/Renderer.h"

#include "glad/gl.h"

namespace SmartGL
{
    namespace Core
    {
        Application *Application::s_Instance = nullptr;

        Application::Application(ApplicationSpecification specification)
            : m_Specification(specification)
        {

            SMART_ASSERT(!s_Instance, "Application already exists!");

            s_Instance = this;

            m_Window = CreateUnique<Window>(WindowProps(specification.Name, specification.Width, specification.Height));
            m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

            Renderer::Init();

            m_UILayer = new UI::UILayer();
            PushOverlay(m_UILayer);
        }

        Application::~Application()
        {
        }

        void Application::Run()
        {

            while (m_Running)
            {
                float time = Time::GetTime();
                Timestep timestep = time - m_LastFrameTime;
                m_LastFrameTime = time;

                for (Layer *layer : m_LayerStack)
                    layer->OnUpdate(timestep);

                m_UILayer->OnUIRenderStart();
                for (Layer *layer : m_LayerStack)
                    layer->OnUIRender();
                m_UILayer->OnUIRenderEnd();

                m_Window->OnUpdate();
            }
        }

        void Application::PushLayer(Layer *layer)
        {
            m_LayerStack.PushLayer(layer);
            layer->OnAttach();
        }

        void Application::PushOverlay(Layer *overlay)
        {
            m_LayerStack.PushOverlay(overlay);
            overlay->OnAttach();
        }

        void Application::OnEvent(Events::Event &e)
        {
            Events::EventDispatcher dispatcher(e);
            dispatcher.Dispatch<Events::WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
            dispatcher.Dispatch<Events::WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

            for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
            {
                (*--it)->OnEvent(e);
                if (e.IsHandled)
                    break;
            }
        }

        bool Application::OnWindowClose(Events::WindowCloseEvent &e)
        {
            m_Running = false;
            Renderer::Shutdown();
            return true;
        }

        bool Application::OnWindowResize(Events::WindowResizeEvent &e)
        {
            if (e.GetWidth() == 0 || e.GetHeight() == 0)
            {
                m_Minimized = true;
                return false;
            }

            m_Minimized = false;
            Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

            return false;
        }
    }
}
