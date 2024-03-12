#pragma once

#include "glm/glm.hpp"

namespace SmartGL
{
    class Random
    {
    public:
        static void Init();
        static float Float();
        static float Float(float min, float max);
        static int Int();
        static int Int(int min, int max);
        static glm::vec2 Vec2(float min, float max);
        static glm::vec3 Vec3();
        static glm::vec3 Vec3(float min, float max);
    };
}
