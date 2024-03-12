#pragma once

#include "Events/Event.h"

namespace SmartGL
{
    namespace Events
    {

        using MouseCode = uint16_t;

        enum : MouseCode
        {
            // From glfw3.h
            Button0 = 0,
            Button1 = 1,
            Button2 = 2,
            Button3 = 3,
            Button4 = 4,
            Button5 = 5,
            Button6 = 6,
            Button7 = 7,

            ButtonLast = Button7,
            ButtonLeft = Button0,
            ButtonRight = Button1,
            ButtonMiddle = Button2
        };

        struct MousePosition
        {
            float x;
            float y;
        };

        struct MouseOffset
        {
            float x;
            float y;
        };

        class MouseMovedEvent : public Event
        {
        public:
            MouseMovedEvent(float x, float y)
            {
                m_MousePosition.x = x;
                m_MousePosition.y = y;
            }

            inline float GetX() const { return m_MousePosition.x; }
            inline float GetY() const { return m_MousePosition.y; }

            std::string ToString() const override
            {
                std::stringstream ss;
                ss << "Mouse moved event: " << m_MousePosition.x << ", " << m_MousePosition.y;
                return ss.str();
            }

            EVENT_CLASS_TYPE(MouseMoved)

        private:
            MousePosition m_MousePosition;
        };

        /**
         * @brief Mouse scrolled event
         *
         */
        class MouseScrolledEvent : public Event
        {
        public:
            MouseScrolledEvent(float xOffset, float yOffset)
            {
                m_MouseOffset.x = xOffset;
                m_MouseOffset.y = yOffset;
            }

            inline float GetXOffset() const { return m_MouseOffset.x; }
            inline float GetYOffset() const { return m_MouseOffset.y; }

            std::string ToString() const override
            {
                std::stringstream ss;
                ss << "Mouse scrolled event: " << GetXOffset() << ", " << GetYOffset();
                return ss.str();
            }

            EVENT_CLASS_TYPE(MouseScrolled)

        private:
            MouseOffset m_MouseOffset;
        };

        /**
         * @brief Mouse button event
         *
         */
        class MouseButtonEvent : public Event
        {
        public:
            inline MouseCode GetMouseButton() const { return m_Button; }

        protected:
            MouseButtonEvent(int button)
                : m_Button(button) {}

            MouseCode m_Button;
        };

        class MouseButtonPressedEvent : public MouseButtonEvent
        {
        public:
            MouseButtonPressedEvent(MouseCode button)
                : m_Button(button), MouseButtonEvent(button) {}

            std::string ToString() const override
            {
                std::stringstream ss;
                ss << "Mouse button pressed event: " << m_Button;
                return ss.str();
            }

            EVENT_CLASS_TYPE(MouseButtonPressed)

        private:
            MouseCode m_Button;
        };

        class MouseButtonReleasedEvent : public MouseButtonEvent
        {
        public:
            MouseButtonReleasedEvent(MouseCode button)
                : m_Button(button), MouseButtonEvent(button) {}

            std::string ToString() const override
            {
                std::stringstream ss;
                ss << "Mouse button released event: " << m_Button;
                return ss.str();
            }

            EVENT_CLASS_TYPE(MouseButtonReleased)

        private:
            MouseCode m_Button;
        };
    }
}
