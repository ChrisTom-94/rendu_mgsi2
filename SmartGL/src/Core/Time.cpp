#include "precompiled.h"

#include "Core/Time.h"

#include "GLFW/glfw3.h"

namespace SmartGL
{
    float Time::GetTime()
    {
        return (float)glfwGetTime();
    }
}