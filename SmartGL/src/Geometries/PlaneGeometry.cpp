#include "precompiled.h"

#include "Geometries/PlaneGeometry.h"

namespace SmartGL
{
    PlaneGeometry::PlaneGeometry(PlaneGeometrySettings settings)
        : m_Settings(settings)
    {

        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        float widthHalf = settings.Width / 2.0f;
        float heightHalf = settings.Height / 2.0f;

        float segmentWidth = settings.Width / settings.WidthSegments;
        float segmentHeight = settings.Height / settings.HeightSegments;

        for (uint8_t iy = 0; iy <= settings.HeightSegments; iy++)
        {
            float y = iy * segmentHeight - heightHalf;

            for (uint8_t ix = 0; ix <= settings.WidthSegments; ix++)
            {
                float x = ix * segmentWidth - widthHalf;

                m_Vertices.push_back({x, -y, 0.0f});

                m_Normals.push_back({0.0f, 0.0f, 1.0f});

                m_UVs.push_back({(float)ix / settings.WidthSegments, 1.0f - (float)iy / settings.HeightSegments});
            }
        }

        for (uint8_t iy = 0; iy < settings.HeightSegments; iy++)
        {
            for (uint8_t ix = 0; ix < settings.WidthSegments; ix++)
            {
                uint32_t a = (settings.WidthSegments + 1) * iy + ix;
                uint32_t b = (settings.WidthSegments + 1) * (iy + 1) + ix;
                uint32_t c = (settings.WidthSegments + 1) * (iy + 1) + (ix + 1);
                uint32_t d = (settings.WidthSegments + 1) * iy + (ix + 1);

                m_Indices.push_back(a);
                m_Indices.push_back(b);
                m_Indices.push_back(d);

                m_Indices.push_back(b);
                m_Indices.push_back(c);
                m_Indices.push_back(d);
            }
        }
    }

    PlaneGeometry::~PlaneGeometry()
    {
    }
}
