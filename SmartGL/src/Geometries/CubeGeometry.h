#pragma once

#include "Geometries/Geometry.h"

#include "glm/glm.hpp"

namespace SmartGL
{

    struct CubeGeometrySettings
    {
        float Width = 1.0f;
        float Height = 1.0f;
        float Depth = 1.0f;
        uint8_t WidthSegments = 1;
        uint8_t HeightSegments = 1;
        uint8_t DepthSegments = 1;
    };

    class CubeGeometry : public Geometry
    {
    public:
        CubeGeometry();
        CubeGeometry(const CubeGeometrySettings &settings);
        ~CubeGeometry() override;

    private:
        void BuildPlane(
            uint8_t u,
            uint8_t v,
            uint8_t w,
            int uDirection,
            int vDirection,
            float width,
            float height,
            float depth,
            uint8_t gridX,
            uint8_t gridY
        );

    private:
        CubeGeometrySettings m_Settings;
    };
}
