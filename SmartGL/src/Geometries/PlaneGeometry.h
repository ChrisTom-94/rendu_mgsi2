#pragma once

#include "Geometries/Geometry.h"

#include "glm/glm.hpp"

namespace SmartGL
{

    struct PlaneGeometrySettings
    {
        float Width = 1.0f;
        float Height = 1.0f;
        uint8_t WidthSegments = 1;
        uint8_t HeightSegments = 1;
    };

    class PlaneGeometry : public Geometry
    {
        public:
            PlaneGeometry(PlaneGeometrySettings settings = PlaneGeometrySettings());
            ~PlaneGeometry() override;

            PlaneGeometrySettings GetSettings() const { return m_Settings; }

        private:
            PlaneGeometrySettings m_Settings;
    };
}
