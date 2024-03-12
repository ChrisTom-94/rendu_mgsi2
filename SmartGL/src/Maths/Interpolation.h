#pragma once

#include "glm/glm.hpp"

namespace SmartGL
{
    namespace Maths
    {

        static float Lerp(float a, float b, float t)
        {
            return a + t * (b - a);
        };

        static glm::vec2 Lerp(const glm::vec2 &a, const glm::vec2 &b, float t)
        {
            return a + t * (b - a);
        };

        static glm::vec3 Lerp(const glm::vec3 &a, const glm::vec3 &b, float t)
        {
            return a + t * (b - a);
        };

        static glm::vec4 Lerp(const glm::vec4 &a, const glm::vec4 &b, float t)
        {
            return a + t * (b - a);
        };
    }
}
