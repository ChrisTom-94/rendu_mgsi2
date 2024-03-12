#pragma once

#include "SmartGL.h"

#include "glm/glm.hpp"
#include <vector>

struct FractalMountainSettings
{
    uint8_t Iterations = 1;
    uint8_t Seed = 0;
    float Roughness = 5.0f;
    float Size = 10.0f;

    FractalMountainSettings() = default;
    FractalMountainSettings(uint8_t iterations, float roughness, float size)
        : Iterations(iterations), Roughness(roughness), Size(size) {}
};

class FractalMountain
{

public:
    FractalMountain() = default;
    FractalMountain(FractalMountainSettings settings);
    ~FractalMountain();

    void Generate();

    inline void SetIterations(uint8_t iterations) { m_Settings.Iterations = iterations; }
    inline void SetRoughness(float roughness) { m_Settings.Roughness = roughness; }
    inline void SetSize(float size) { m_Settings.Size = size; }
    inline int GetIterations() const { return m_Settings.Iterations; }
    inline float GetRoughness() const { return m_Settings.Roughness; }
    inline float GetSize() const { return m_Settings.Size; }

    inline std::vector<glm::vec3> GetVertices() const { return m_Vertices; };
    inline std::vector<glm::vec3> GetNormals() const { return m_Normals; }

    inline std::vector<float> GetAttributes() const
    {
        std::vector<float> attributes;
        for (int i = 0; i < m_Vertices.size(); i++)
        {
            attributes.push_back(m_Vertices[i].x);
            attributes.push_back(m_Vertices[i].y);
            attributes.push_back(m_Vertices[i].z);
            attributes.push_back(m_Normals[i].x);
            attributes.push_back(m_Normals[i].y);
            attributes.push_back(m_Normals[i].z);
        }
        return attributes;
    }

    inline std::vector<uint32_t> GetIndices() const { return m_Indices; }

    inline float GetMaxHeight() const {
        float max = 0.0f;
        for (auto vertex : m_Vertices)
            if (vertex.z > max)
                max = vertex.z;
        return max;
    }


private:
    void ComputeNormals();
    void SquareStep(int x, int z, int step);
    void DiamondStep(int x, int z, int step);
    float ComputeRandomDisplacement(float distance);

private:
    FractalMountainSettings m_Settings;
    std::vector<glm::vec3> m_Vertices;
    std::vector<glm::vec3> m_Normals;
    std::vector<uint32_t> m_Indices;
};
