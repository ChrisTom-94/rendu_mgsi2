#pragma once

#include "glm/glm.hpp"

namespace SmartGL
{
    struct Material
    {
        glm::vec3 Albedo = glm::vec3(1.0f);
        float Shininess = 32.0f;

        Material() = default;
        Material(const glm::vec3 &albedo, float shininess)
            : Albedo(albedo), Shininess(shininess) {}
    };
}
