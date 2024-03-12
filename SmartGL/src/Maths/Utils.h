#pragma once

namespace SmartGL
{
    namespace Maths
    {
        static float Clamp(float value, float min, float max)
        {
            return value < min ? min : (value > max ? max : value);
        }
    }
}
