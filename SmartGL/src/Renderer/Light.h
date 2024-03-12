#pragma once

#include <glm/glm.hpp>

namespace SmartGL
{
    struct Light
    {
        glm::vec3 AmbientColor = glm::vec3(0.1f);
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Color = glm::vec3(1.0f);
        float Intensity = 1.0f;

        Light() = default;

        Light(const glm::vec3 &position, const glm::vec3 &color, float intensity)
            : Position(position), Color(color), Intensity(intensity) {}
    };


}
