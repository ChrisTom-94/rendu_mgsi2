#include "precompiled.h"

#include "Geometries/CubeGeometry.h"

#include "Geometries/PlaneGeometry.h"

#include "Core/Log.h"

namespace SmartGL
{

    static uint8_t nbVertices = 0;

    CubeGeometry::CubeGeometry()
    {
        auto [Width, Height, Depth, WidthSegments, HeightSegments, DepthSegments] = m_Settings;

        uint8_t x = 0, y = 1, z = 2;
        int xDirection = 1, yDirection = 1, zDirection = 1;

        BuildPlane('z', 'y', 'x', -1, -1, Depth, Height, Width, DepthSegments, HeightSegments);
        BuildPlane('z', 'y', 'x', 1, -1, Depth, Height, -Width, DepthSegments, HeightSegments);
        BuildPlane('x', 'z', 'y', 1, 1, Width, Depth, Height, WidthSegments, DepthSegments);
        BuildPlane('x', 'z', 'y', 1, -1, Width, Depth, -Height, WidthSegments, DepthSegments);
        BuildPlane('x', 'y', 'z', 1, -1, Width, Height, Depth, WidthSegments, HeightSegments);
        BuildPlane('x', 'y', 'z', -1, -1, Width, Height, -Depth, WidthSegments, HeightSegments);
    }

    CubeGeometry::CubeGeometry(const CubeGeometrySettings &settings)
    {
        m_Settings = settings;

        auto [Width, Height, Depth, WidthSegments, HeightSegments, DepthSegments] = m_Settings;

        uint8_t x = 0, y = 1, z = 2;
        int xDirection = 1, yDirection = 1, zDirection = 1;

        BuildPlane('z', 'y', 'x', -1, -1, Depth, Height, Width, DepthSegments, HeightSegments);
        BuildPlane('z', 'y', 'x', 1, -1, Depth, Height, -Width, DepthSegments, HeightSegments);
        BuildPlane('x', 'z', 'y', 1, 1, Width, Depth, Height, WidthSegments, DepthSegments);
        BuildPlane('x', 'z', 'y', 1, -1, Width, Depth, -Height, WidthSegments, DepthSegments);
        BuildPlane('x', 'y', 'z', 1, -1, Width, Height, Depth, WidthSegments, HeightSegments);
        BuildPlane('x', 'y', 'z', -1, -1, Width, Height, -Depth, WidthSegments, HeightSegments);
    }

    CubeGeometry::~CubeGeometry()
    {
    }

    void CubeGeometry::BuildPlane(
        uint8_t u, uint8_t v, uint8_t w,
        int uDirection, int vDirection,
        float width, float height, float depth,
        uint8_t gridX, uint8_t gridY)
    {

        uint8_t segmentWidth = width / gridX;
        uint8_t segmentHeight = height / gridY;

        float widthHalf = width / 2;
        float heightHalf = height / 2;
        float depthHalf = depth / 2;

        float gridX1 = gridX + 1;
        float gridY1 = gridY + 1;

        glm::vec3 vector;

        for (uint8_t iy = 0; iy < gridY1; iy++)
        {
            float y = iy * segmentHeight - heightHalf;

            for (uint8_t ix = 0; ix < gridX1; ix++)
            {
                float x = ix * segmentWidth - widthHalf;

                if (u == 'z')
                    vector.z = x * uDirection;
                else if (u == 'x')
                    vector.x = x * uDirection;
                else if (u == 'y')
                    vector.y = x * uDirection;

                if (v == 'z')
                    vector.z = y * vDirection;
                else if (v == 'x')
                    vector.x = y * vDirection;
                else if (v == 'y')
                    vector.y = y * vDirection;

                if (w == 'z')
                    vector.z = depthHalf;
                else if (w == 'x')
                    vector.x = depthHalf;
                else if (w == 'y')
                    vector.y = depthHalf;

                m_Vertices.push_back(vector);

                if (u == 'z')
                    vector.z = 0.0f;
                else if (u == 'x')
                    vector.x = 0.0f;
                else if (u == 'y')
                    vector.y = 0.0f;

                if (v == 'z')
                    vector.z = 0.0f;
                else if (v == 'x')
                    vector.x = 0.0f;
                else if (v == 'y')
                    vector.y = 0.0f;

                if (w == 'z')
                    vector.z = depth > 0 ? 1.0f : -1.0f;
                else if (w == 'x')
                    vector.x = depth > 0 ? 1.0f : -1.0f;
                else if (w == 'y')
                    vector.y = depth > 0 ? 1.0f : -1.0f;

                m_Normals.push_back(vector);

                m_UVs.push_back(glm::vec2(ix / gridX, 1 - (iy / gridY)));
            }
        }

        for (uint8_t iy = 0; iy < gridY; iy++)
        {
            for (uint8_t ix = 0; ix < gridX; ix++)
            {
                uint8_t a = nbVertices + ix + gridX1 * iy;
                uint8_t b = nbVertices + ix + gridX1 * (iy + 1);
                uint8_t c = nbVertices + (ix + 1) + gridX1 * (iy + 1);
                uint8_t d = nbVertices + (ix + 1) + gridX1 * iy;

                m_Indices.push_back(a);
                m_Indices.push_back(b);
                m_Indices.push_back(d);

                m_Indices.push_back(b);
                m_Indices.push_back(c);
                m_Indices.push_back(d);
            }
        }

        nbVertices += gridX1 * gridY1;
    }
}
