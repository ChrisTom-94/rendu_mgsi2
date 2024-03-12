#include "precompiled.h"

#include "Geometries/TorusGeometry.h"

namespace SmartGL
{
    TorusGeometry::TorusGeometry(TorusGeometrySettings settings)
        : m_Settings(settings)
    {
        float radius = settings.Radius;
        float tube = settings.Tube;
        uint8_t radialSegments = settings.RadialSegments;
        uint8_t tubularSegments = settings.TubularSegments;

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;
        std::vector<uint32_t> indices;

        glm::vec3 center = {0.0f, 0.0f, 0.0f};

        for (uint8_t j = 0; j <= radialSegments; j++)
        {
            for (uint8_t i = 0; i <= tubularSegments; i++)
            {
                float u = (float)i / tubularSegments * settings.Arc;
                float v = (float)j / radialSegments * glm::two_pi<float>();

                float x = (radius + tube * glm::cos(v)) * glm::cos(u);
                float y = (radius + tube * glm::cos(v)) * glm::sin(u);
                float z = tube * glm::sin(v);

                vertices.push_back({x, y, z});

                center.x = radius * glm::cos(u);
                center.y = radius * glm::sin(u);
                normals.push_back(glm::normalize(vertices.back() - center));

                uvs.push_back({(float)i / tubularSegments, (float)j / radialSegments});
            }
        }

        for (uint8_t j = 1; j <= radialSegments; j++)
        {
            for (uint8_t i = 1; i <= tubularSegments; i++)
            {
                uint32_t a = (tubularSegments + 1) * j + i - 1;
                uint32_t b = (tubularSegments + 1) * (j - 1) + i - 1;
                uint32_t c = (tubularSegments + 1) * (j - 1) + i;
                uint32_t d = (tubularSegments + 1) * j + i;

                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);

                indices.push_back(c);
                indices.push_back(d);
                indices.push_back(a);
            }
        }

        m_Vertices = vertices;
        m_Normals = normals;
        m_UVs = uvs;

        m_Indices = indices;
    }

    TorusGeometry::~TorusGeometry()
    {
    }

}
