#pragma once

#include "Core/Time.h"

#include "Events/Event.h"

#include <string>

namespace SmartGL
{
    namespace Core
    {
        class Layer
        {
        public:
            Layer(const std::string &name = "Layer");
            virtual ~Layer();

            /**
             * @brief Called when the layer is attached to the application
             * @note Each openGL initialization should be done here
            */
            virtual void OnAttach() {}

            /**
             * @brief Called when the layer is detached from the application
             * @note Each openGL cleanup should be done here
            */
            virtual void OnDetach() {}

            /**
             * @brief Called each frame
             * @note Draw calls should be done here
             * @param ts The time step since the last update
            */
            virtual void OnUpdate(Timestep ts) {}

            /**
             * @brief Called each frame
             * @note UI draw calls should be done here
            */
            virtual void OnUIRender() {}

            /**
             * @brief Called when an event is triggered
             * @param event The event that was triggered
            */
            virtual void OnEvent(Events::Event &event) {}

            inline const std::string &GetName() const { return m_Name; }

        protected:
            std::string m_Name;
        };
    }
}
