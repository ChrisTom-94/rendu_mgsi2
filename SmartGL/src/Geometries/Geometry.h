#pragma once

#include "glm/glm.hpp"

#include <vector>

namespace SmartGL
{
    class Geometry
    {
        public:
            Geometry() = default;
            virtual ~Geometry() = default;

            inline const std::vector<glm::vec3> GetVertices()  const{ return m_Vertices; }
            inline const std::vector<glm::vec3> GetNormals() const { return m_Normals; }
            inline const std::vector<glm::vec2> GetUVs() const { return m_UVs; }
            inline const std::vector<uint32_t> GetIndices() const { return m_Indices; }

            std::vector<float> GetAttributes() const
            {
                std::vector<float> attributes;
                for (uint32_t i = 0; i < m_Vertices.size(); i++)
                {
                    attributes.push_back(m_Vertices[i].x);
                    attributes.push_back(m_Vertices[i].y);
                    attributes.push_back(m_Vertices[i].z);
                    attributes.push_back(m_Normals[i].x);
                    attributes.push_back(m_Normals[i].y);
                    attributes.push_back(m_Normals[i].z);
                    attributes.push_back(m_UVs[i].x);
                    attributes.push_back(m_UVs[i].y);
                }
                return attributes;
            }

        protected:
            std::vector<glm::vec3> m_Vertices;
            std::vector<glm::vec3> m_Normals;
            std::vector<glm::vec2> m_UVs;
            std::vector<uint32_t> m_Indices;
    };
}
