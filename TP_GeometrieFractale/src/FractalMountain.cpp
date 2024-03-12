#include "FractalMountain.h"

FractalMountain::FractalMountain(FractalMountainSettings settings)
    : m_Settings(settings)
{
    SmartGL::PlaneGeometrySettings planeSettings;
    planeSettings.Width = m_Settings.Size;
    planeSettings.Height = m_Settings.Size;
    planeSettings.WidthSegments = 1 << m_Settings.Iterations;
    planeSettings.HeightSegments = planeSettings.WidthSegments;
    SmartGL::PlaneGeometry plane(planeSettings);

    m_Vertices = plane.GetVertices();
    m_Normals = plane.GetNormals();
    m_Indices = plane.GetIndices();
    Generate();
}

FractalMountain::~FractalMountain()
{
}

void FractalMountain::Generate()
{
    SmartGL::PlaneGeometrySettings planeSettings;
    planeSettings.Width = m_Settings.Size;
    planeSettings.Height = m_Settings.Size;
    planeSettings.WidthSegments = 1 << m_Settings.Iterations;
    planeSettings.HeightSegments = planeSettings.WidthSegments;
    SmartGL::PlaneGeometry plane(planeSettings);

    m_Vertices = plane.GetVertices();
    m_Normals = plane.GetNormals();
    m_Indices = plane.GetIndices();

    { // diamond square loop
        uint32_t gridSize = (1 << m_Settings.Iterations) + 1;
        int rowCount = gridSize;

        while (rowCount > 1)
        {
            int step = rowCount / 2;

            for (float x = step; x < gridSize; x += rowCount)
                for (float y = step; y < gridSize; y += rowCount)
                    SquareStep(x, y, step);

            float offset = 0.0f;
            for (float x = 0; x < gridSize; x += step)
            {
                offset = (offset == 0.0f) ? step : 0.0f;
                for (float y = offset; y < gridSize; y += rowCount)
                    DiamondStep(x, y, step);
            }
            rowCount = step;
        }
    }

    ComputeNormals();
}

void FractalMountain::SquareStep(int x, int y, int step)
{
    uint32_t gridSize = (1 << m_Settings.Iterations) + 1;
    float average = 0.0f;
    int count = 0;

    average += m_Vertices[(x - step) + (y - step) * gridSize].z;
    average += m_Vertices[(x + step) + (y - step) * gridSize].z;
    average += m_Vertices[(x - step) + (y + step) * gridSize].z;
    average += m_Vertices[(x + step) + (y + step) * gridSize].z;

    average /= 4;
    float distance = step * 2;
    m_Vertices[x + y * gridSize].z = average + ComputeRandomDisplacement(distance);
}

void FractalMountain::DiamondStep(int x, int y, int step)
{
    uint32_t gridSize = (1 << m_Settings.Iterations) + 1;
    float average = 0.0f;
    int count = 0;

    if (x - step >= 0)
    {
        average += m_Vertices[(x - step) + y * gridSize].z;
        count++;
    }

    if (x + step < m_Vertices.size())
    {
        average += m_Vertices[(x + step) + y * gridSize].z;
        count++;
    }

    if (y - step >= 0)
    {
        average += m_Vertices[x + (y - step) * gridSize].z;
        count++;
    }

    if (y + step < m_Vertices.size())
    {
        average += m_Vertices[x + (y + step) * gridSize].z;
        count++;
    }

    average /= count;
    float distance = step * 2;
    m_Vertices[x + y * gridSize].z = average + ComputeRandomDisplacement(distance);
}

float FractalMountain::ComputeRandomDisplacement(float distance)
{
    return SmartGL::Random::Float(-1.0f, 1.0f) * distance * glm::pow(2.0f, -m_Settings.Roughness);
}

void FractalMountain::ComputeNormals()
{
    m_Normals.resize(m_Vertices.size(), glm::vec3(0.0f));

    for(int i = 0; i < m_Indices.size(); i += 3)
    {
        uint32_t index0 = m_Indices[i];
        uint32_t index1 = m_Indices[i + 1];
        uint32_t index2 = m_Indices[i + 2];

        glm::vec3 v0 = m_Vertices[index0];
        glm::vec3 v1 = m_Vertices[index1];
        glm::vec3 v2 = m_Vertices[index2];

        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        m_Normals[index0] += normal;
        m_Normals[index1] += normal;
        m_Normals[index2] += normal;
    }
}
