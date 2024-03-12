#pragma once

#include "Events/Event.h"

namespace SmartGL
{
    namespace Events
    {
        using KeyCode = uint16_t;

        typedef unsigned int KeyCountRepeat;

        namespace Key
        {
            enum : KeyCode
            {
                // From glfw3.h
                Space = 32,
                Apostrophe = 39, /* ' */
                Comma = 44,      /* , */
                Minus = 45,      /* - */
                Period = 46,     /* . */
                Slash = 47,      /* / */

                D0 = 48, /* 0 */
                D1 = 49, /* 1 */
                D2 = 50, /* 2 */
                D3 = 51, /* 3 */
                D4 = 52, /* 4 */
                D5 = 53, /* 5 */
                D6 = 54, /* 6 */
                D7 = 55, /* 7 */
                D8 = 56, /* 8 */
                D9 = 57, /* 9 */

                Semicolon = 59, /* ; */
                Equal = 61,     /* = */

                A = 65,
                B = 66,
                C = 67,
                D = 68,
                E = 69,
                F = 70,
                G = 71,
                H = 72,
                I = 73,
                J = 74,
                K = 75,
                L = 76,
                M = 77,
                N = 78,
                O = 79,
                P = 80,
                Q = 81,
                R = 82,
                S = 83,
                T = 84,
                U = 85,
                V = 86,
                W = 87,
                X = 88,
                Y = 89,
                Z = 90,

                LeftBracket = 91,  /* [ */
                Backslash = 92,    /* \ */
                RightBracket = 93, /* ] */
                GraveAccent = 96,  /* ` */

                World1 = 161, /* non-US #1 */
                World2 = 162, /* non-US #2 */

                /* Function keys */
                Escape = 256,
                Enter = 257,
                Tab = 258,
                Backspace = 259,
                Insert = 260,
                Delete = 261,
                Right = 262,
                Left = 263,
                Down = 264,
                Up = 265,
                PageUp = 266,
            };
        }

        class KeyEvent : public Event
        {
        public:
            inline KeyCode GetKeyCode() const { return m_KeyCode; }

        protected:
            KeyEvent(KeyCode keycode)
                : m_KeyCode(keycode) {}

            KeyCode m_KeyCode;
        };

        /**
         * @brief Key pressed event
         *
         */
        class KeyPressedEvent : public KeyEvent
        {
        public:
            KeyPressedEvent(KeyCode keycode, KeyCountRepeat repeatCount)
                : KeyEvent(keycode), m_RepeatCount(repeatCount) {}

            inline int GetRepeatCount() const { return m_RepeatCount; }

            std::string ToString() const override
            {
                std::stringstream ss;
                ss << "Key pressed event: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
                return ss.str();
            }

            EVENT_CLASS_TYPE(KeyPressed)

        private:
            KeyCountRepeat m_RepeatCount;
        };

        /**
         * @brief Key released event
         *
         */
        class KeyReleasedEvent : public KeyEvent
        {
        public:
            KeyReleasedEvent(KeyCode keycode)
                : KeyEvent(keycode) {}

            std::string ToString() const override
            {
                std::stringstream ss;
                ss << "Key released event: " << m_KeyCode;
                return ss.str();
            }

            EVENT_CLASS_TYPE(KeyReleased)
        };

        /**
         * @brief Key typed event
         *
         */
        class KeyTypedEvent : public KeyEvent
        {
        public:
            KeyTypedEvent(KeyCode keycode)
                : KeyEvent(keycode) {}

            std::string ToString() const override
            {
                std::stringstream ss;
                ss << "Key typed event: " << m_KeyCode;
                return ss.str();
            }

            EVENT_CLASS_TYPE(KeyTyped)
        };
    }
}
