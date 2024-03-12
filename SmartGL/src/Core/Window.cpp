#include "precompiled.h"

#include "Core/Window.h"
#include "Core/Assert.h"

#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvent.h"

#include "GLFW/glfw3.h"

namespace SmartGL
{
    namespace Core
    {
        static uint8_t s_GLFWWindowCount = 0;

        static void GLFWErrorCallback(int error, const char *description)
        {
            SMART_LOG_ERROR("GLFW Error ({0}): {1}", error, description);
        }

        Window::Window(const Core::WindowProps &properties)
        {
            Init(properties);
        }

        Window::~Window()
        {
            Shutdown();
        }

        void Window::Init(const Core::WindowProps &props)
        {
            m_Data.Title = props.Title;
            m_Data.Width = props.Width;
            m_Data.Height = props.Height;

            if (s_GLFWWindowCount == 0)
            {
                int success = glfwInit();
                SMART_ASSERT(success, "Could not initialize GLFW!");
                glfwSetErrorCallback(GLFWErrorCallback);
            }

            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

            m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
            ++s_GLFWWindowCount;

            m_Context = CreateUnique<GraphicsContext>(m_Window);
            m_Context->Init();

            glfwSetWindowUserPointer(m_Window, &m_Data);
            SetVSync(true);

            glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width, int height)
                                      {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                data.Width = width;
                data.Height = height;

                Events::WindowResizeEvent event(width, height);
                data.EventCallback(event); });

            glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window)
                                       {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                Events::WindowCloseEvent event;
                data.EventCallback(event); });

            glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
                               {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                switch (action)
                {
                case GLFW_PRESS:
                {
                    Events::KeyPressedEvent event(key, 0);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    Events::KeyReleasedEvent event(key);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    Events::KeyPressedEvent event(key, 1);
                    data.EventCallback(event);
                    break;
                }
                } });

            glfwSetCharCallback(m_Window, [](GLFWwindow *window, unsigned int keycode)
                                {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                Events::KeyTypedEvent event(keycode);
                data.EventCallback(event); });

            glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods)
                                       {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                switch (action)
                {
                    case GLFW_PRESS:
                    {
                        Events::MouseButtonPressedEvent event(button);
                        data.EventCallback(event);
                        break;
                    }

                    case GLFW_RELEASE:
                    {
                        Events::MouseButtonReleasedEvent event(button);
                        data.EventCallback(event);
                        break;
                    }
                } });

            glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xOffset, double yOffset)
                                  {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                Events::MouseScrolledEvent event((float)xOffset, (float)yOffset);
                data.EventCallback(event); });

            glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xPos, double yPos)
                                     {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                Events::MouseMovedEvent event((float)xPos, (float)yPos);
                data.EventCallback(event); });
        }

        void Window::Shutdown()
        {
            glfwDestroyWindow(m_Window);
            --s_GLFWWindowCount;

            if (s_GLFWWindowCount == 0)
                glfwTerminate();
        }

        void Window::OnUpdate()
        {
            glfwPollEvents();
            m_Context->SwapBuffers();
        }

        void Window::SetVSync(bool enabled)
        {
            if (enabled)
                glfwSwapInterval(1);
            else
                glfwSwapInterval(0);

            m_Data.VSync = enabled;
        }

        bool Window::IsVSync() const
        {
            return m_Data.VSync;
        }
    }
}
