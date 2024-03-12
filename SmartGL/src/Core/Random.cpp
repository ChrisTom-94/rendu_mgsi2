#include "Core/Random.h"

#include <stdlib.h>
#include <time.h>

namespace SmartGL
{
    void Random::Init()
    {
        srand(time(NULL));
    }

    float Random::Float()
    {
        return (float)rand() / (float)RAND_MAX;
    }

    float Random::Float(float min, float max)
    {
        return min + (max - min) * Float();
    }

    int Random::Int()
    {
        return rand();
    }

    int Random::Int(int min, int max)
    {
        return min + rand() % (max - min);
    }

    glm::vec2 Random::Vec2(float min, float max)
    {
        return glm::vec2(Float(min, max), Float(min, max));
    }

    glm::vec3 Random::Vec3()
    {
        return glm::vec3(Float(), Float(), Float());
    }

    glm::vec3 Random::Vec3(float min, float max)
    {
        return glm::vec3(Float(min, max), Float(min, max), Float(min, max));
    }
}
