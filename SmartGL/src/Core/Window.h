#pragma once

#include "Renderer/GraphicsContext.h"

#include "Events/Event.h"

#include <functional>
#include <string>

class GLFWwindow;

namespace SmartGL
{
    namespace Core
    {
        struct WindowProps
        {
            std::string Title;
            unsigned int Width;
            unsigned int Height;

            WindowProps(const std::string &title = "OpenGL window",
                        unsigned int width = 1280,
                        unsigned int height = 720)
                : Title(title), Width(width), Height(height)
            {
            }
        };

        class Window
        {
        public:
            using EventCallbackFn = std::function<void(Events::Event &)>;

            Window(const Core::WindowProps &properties);
            ~Window();

            void OnUpdate();

            inline unsigned int GetWidth() const { return m_Data.Width; }
            inline unsigned int GetHeight() const { return m_Data.Height; }

            inline void SetEventCallback(const EventCallbackFn &callback) { m_Data.EventCallback = callback; }
            void SetVSync(bool enabled);
            bool IsVSync() const;

            inline void *GetNativeWindow() const { return m_Window; }

            inline const float GetAspectRatio() const
            {
                return (float)m_Data.Width / (float)m_Data.Height;
            }

        private:
            void Init(const Core::WindowProps &props);
            void Shutdown();

        private:
            GLFWwindow *m_Window;
            Unique<GraphicsContext> m_Context;

            struct WindowData
            {
                std::string Title;
                unsigned int Width, Height;
                bool VSync;

                EventCallbackFn EventCallback;
            };

            WindowData m_Data;
        };
    }
}
