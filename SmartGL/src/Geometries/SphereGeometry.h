#pragma once

#include "Geometries/Geometry.h"

#include "glm/glm.hpp"

namespace SmartGL
{

    struct SphereGeometrySettings
    {
        float Radius = 1.0f;
        uint8_t WidthSegments = 8;
        uint8_t HeightSegments = 6;
    };

    class SphereGeometry : public Geometry
    {
        public:
            SphereGeometry(SphereGeometrySettings settings = SphereGeometrySettings());
            ~SphereGeometry() override;

        private:
            SphereGeometrySettings m_Settings;
    };

}
