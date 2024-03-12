#pragma once

#include "Core/Window.h"

#include "Events/Event.h"

namespace SmartGL
{
    namespace Events
    {
        using WindowCode = uint16_t;

        struct WindowSize
        {
            uint32_t Width;
            uint32_t Height;
        };

        enum : WindowCode
        {
            // From glfw3.h
            Close = 0x0001,
            Resize = 0x0002,
            Focus = 0x0004,
            Iconify = 0x0008,
            Maximize = 0x0010,
            FramebufferResize = 0x0020,
            ContentScale = 0x0040
        };

        class WindowResizeEvent : public Event
        {
        public:
            WindowResizeEvent(unsigned int width, unsigned int height)
            {
                m_WindowSize.Width = width;
                m_WindowSize.Height = height;
            }

            inline unsigned int GetWidth() const { return m_WindowSize.Width; }
            inline unsigned int GetHeight() const { return m_WindowSize.Height; }

            std::string ToString() const override
            {
                std::stringstream ss;
                ss << "Window resize event: " << m_WindowSize.Width << ", " << m_WindowSize.Height;
                return ss.str();
            }

            EVENT_CLASS_TYPE(WindowResize)

        private:
            WindowSize m_WindowSize;
        };

        /**
         * @brief Window close event
         *
         */
        class WindowCloseEvent : public Event
        {
        public:
            WindowCloseEvent() {}

            EVENT_CLASS_TYPE(WindowClose)
        };
    }
}
