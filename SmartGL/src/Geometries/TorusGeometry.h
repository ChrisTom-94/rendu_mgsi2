#pragma once

#include "Geometries/Geometry.h"

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

namespace SmartGL
{

    struct TorusGeometrySettings
    {
        float Radius = 1.0f;
        float Tube = 0.4f;
        uint8_t RadialSegments = 8;
        uint8_t TubularSegments = 6;
        float Arc = glm::two_pi<float>();
    };

    class TorusGeometry : public Geometry
    {
        public:
            TorusGeometry(TorusGeometrySettings settings = TorusGeometrySettings());
            ~TorusGeometry() override;

        private:
            TorusGeometrySettings m_Settings;
    };

}
