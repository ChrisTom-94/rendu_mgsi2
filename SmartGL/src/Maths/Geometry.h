#pragma once

#include "glm/glm.hpp"

namespace SmartGL
{
    namespace Maths
    {
        static glm::vec3 ProjectPointOnPlane(const glm::vec3 &point, const glm::vec3 &planeNormal, const glm::vec3 &planePoint)
        {
            glm::vec3 w = point - planePoint;
            float d = glm::dot(planeNormal, w);
            return point - d * planeNormal;
        }

        static float Radians(float degrees)
        {
            return degrees * PI / 180.0f;
        }

        static float Degrees(float radians)
        {
            return radians * 180.0f / PI;
        }
    }
}
