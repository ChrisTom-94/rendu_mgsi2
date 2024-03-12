#include "precompiled.h"

#include "Geometries/SphereGeometry.h"

#include <glm/gtc/constants.hpp>

namespace SmartGL
{
    SphereGeometry::SphereGeometry(SphereGeometrySettings settings)
        : m_Settings(settings)
    {
        float radius = settings.Radius;
        uint8_t widthSegments = settings.WidthSegments;
        uint8_t heightSegments = settings.HeightSegments;

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;
        std::vector<uint32_t> indices;

        for (uint8_t iy = 0; iy <= heightSegments; iy++)
        {
            float v = (float)iy / heightSegments;
            float vPhi = v * glm::pi<float>();

            for (uint8_t ix = 0; ix <= widthSegments; ix++)
            {
                float u = (float)ix / widthSegments;
                float uTheta = u * glm::two_pi<float>();

                float x = -radius * glm::sin(vPhi) * glm::sin(uTheta);
                float y = radius * glm::cos(vPhi);
                float z = radius * glm::sin(vPhi) * glm::cos(uTheta);

                vertices.push_back({x, y, z});

                normals.push_back({x, y, z});

                uvs.push_back({u, 1.0f - v});
            }
        }

        for (uint8_t iy = 0; iy < heightSegments; iy++)
        {
            for (uint8_t ix = 0; ix < widthSegments; ix++)
            {
                uint32_t a = (widthSegments + 1) * iy + ix;
                uint32_t b = (widthSegments + 1) * (iy + 1) + ix;
                uint32_t c = (widthSegments + 1) * (iy + 1) + (ix + 1);
                uint32_t d = (widthSegments + 1) * iy + (ix + 1);

                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(d);

                indices.push_back(b);
                indices.push_back(c);
                indices.push_back(d);
            }
        }

        m_Vertices = vertices;
        m_Normals = normals;
        m_UVs = uvs;
        m_Indices = indices;
    }

    SphereGeometry::~SphereGeometry()
    {
    }
}
