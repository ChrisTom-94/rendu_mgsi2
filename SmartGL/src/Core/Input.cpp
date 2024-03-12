#include "precompiled.h"

#include "Core/Input.h"
#include "Core/Application.h"

#include "GLFW/glfw3.h"

namespace SmartGL
{
    bool Input::IsKeyPressed(const Events::KeyCode key)
    {
        auto *window = static_cast<GLFWwindow *>(Core::Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetKey(window, static_cast<int32_t>(key));
        return state == GLFW_PRESS;
    }

    bool Input::IsKeyReleased(const Events::KeyCode key)
    {
        auto *window = static_cast<GLFWwindow *>(Core::Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetKey(window, static_cast<int32_t>(key));
        return state == GLFW_RELEASE;
    }

    bool Input::IsMouseButtonPressed(const Events::MouseCode button)
    {
        auto *window = static_cast<GLFWwindow *>(Core::Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
        return state == GLFW_PRESS;
    }

    bool Input::IsMouseButtonHeld(const Events::MouseCode button)
    {
        auto *window = static_cast<GLFWwindow *>(Core::Application::Get().GetWindow().GetNativeWindow());
        auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
        return state == GLFW_REPEAT;
    }

    glm::vec2 Input::GetMousePosition()
    {
        auto *window = static_cast<GLFWwindow *>(Core::Application::Get().GetWindow().GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return {(float)xpos, (float)ypos};
    }

    float Input::GetMouseX()
    {
        return GetMousePosition().x;
    }

    float Input::GetMouseY()
    {
        return GetMousePosition().y;
    }
}
