#pragma once

#include <iostream>
#include <sstream>

#define BIND_EVENT_FN(fn) [this](auto &&...args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace SmartGL
{
    namespace Events
    {

        enum class EventType
        {
            None = 0,
            WindowClose,
            WindowResize,
            KeyPressed,
            KeyReleased,
            KeyTyped,
            MouseButtonPressed,
            MouseButtonReleased,
            MouseMoved,
            MouseScrolled
        };

#define EVENT_CLASS_TYPE(type)                      \
    static auto GetStaticType() -> EventType        \
    {                                               \
        return EventType::type;                     \
    }                                               \
    auto GetEventType() const -> EventType override \
    {                                               \
        return GetStaticType();                     \
    }                                               \
    auto GetName() const -> const char *override    \
    {                                               \
        return #type;                               \
    }

        class Event
        {
            friend class EventDispatcher;

        public:
            virtual EventType GetEventType() const = 0;
            virtual const char *GetName() const = 0;
            virtual std::string ToString() const { return GetName(); }

            bool IsHandled = false;
        };

        class EventDispatcher
        {
        public:
            EventDispatcher(Event &event)
                : m_Event(event)
            {
            }

            template <typename T, typename F>
            bool Dispatch(const F &func)
            {
                if (m_Event.GetEventType() == T::GetStaticType())
                {
                    m_Event.IsHandled = func(static_cast<T &>(m_Event));
                    return true;
                }
                return false;
            }

        private:
            Event &m_Event;
        };

        inline std::ostream &operator<<(std::ostream &os, const Event &e)
        {
            return os << e.ToString();
        }
    }
}
