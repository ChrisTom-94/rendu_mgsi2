#pragma once

#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/matrix_decompose.hpp"

namespace SmartGL
{
    namespace Maths
    {
        class Transform
        {

        public:
            Transform() : m_Dirty(true){};
            ~Transform(){};

            inline const glm::vec3 &GetPosition() const { return m_Position; }
            inline const glm::vec3 &GetRotation() const { return m_Rotation; }
            inline const glm::vec3 &GetScale() const { return m_Scale; }

            inline void SetPosition(const glm::vec3 &position)
            {
                m_Position = position;
                m_Dirty = true;
            }

            inline void SetRotation(const glm::vec3 &rotation)
            {
                m_Rotation = rotation;
                m_Orientation = glm::quat(glm::radians(m_Rotation));
                m_Dirty = true;
            }

            inline void SetScale(const glm::vec3 &scale)
            {
                m_Scale = scale;
                m_Dirty = true;
            }

            inline const glm::mat4 &GetLocalMatrix() const { return m_LocalMatrix; }

            inline void SetLocalMatrix(const glm::mat4 &localMatrix)
            {
                m_LocalMatrix = localMatrix;
                glm::vec4 perspective;
                glm::vec3 skew;
                glm::decompose(localMatrix, m_Scale, m_Orientation, m_Position, skew, perspective);
                m_Rotation = glm::eulerAngles(m_Orientation);
                m_Dirty = true;
            }

            inline const glm::quat &GetOrientation() const { return m_Orientation; }

            inline void SetOrientation(const glm::quat &orientation)
            {
                auto originalEuler = m_Rotation;
                m_Orientation = orientation;
                m_Rotation = glm::eulerAngles(m_Orientation);

                if (
                    (fabs(m_Rotation.x - originalEuler.x) == glm::pi<float>()) &&
                    (fabs(m_Rotation.z - originalEuler.z) == glm::pi<float>()))
                {
                    m_Rotation.x = originalEuler.x;
                    m_Rotation.y = glm::pi<float>() - m_Rotation.y;
                    m_Rotation.z = originalEuler.z;
                }

                m_Dirty = true;
            }

            inline void Rotate(const glm::vec3 &axis, float angle)
            {
                m_Orientation = glm::rotate(m_Orientation, angle, axis);
                m_Rotation = glm::eulerAngles(m_Orientation);
                m_Dirty = true;
            }

            inline void Rotate(const glm::quat &rotation)
            {
                m_Orientation = rotation * m_Orientation;
                m_Rotation = glm::eulerAngles(m_Orientation);
                m_Dirty = true;
            }

            inline void Translate(const glm::vec3 &translation)
            {
                m_Position += translation;
                m_Dirty = true;
            }

            inline void Scale(const glm::vec3 &scale)
            {
                m_Scale *= scale;
                m_Dirty = true;
            }

            inline const glm::vec3 &GetUp() const { return m_Up; }
            inline const glm::vec3 &GetRight() const { return m_Right; }
            inline const glm::vec3 &GetForward() const { return m_Forward; }

            void Update()
            {
                if (!m_Dirty)
                    return;

                glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
                                      glm::toMat4(m_Orientation) *
                                      glm::scale(glm::mat4(1.0f), m_Scale);

                m_LocalMatrix = transform;
                if(m_Parent)
                    m_LocalMatrix = m_Parent->GetLocalMatrix() * m_LocalMatrix;

                m_Dirty = false;
            }

        private:
            glm::mat4 m_LocalMatrix = glm::mat4(1.0f);
            glm::quat m_Orientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

            glm::vec3 m_Position = glm::vec3(0.0f);
            glm::vec3 m_Rotation = glm::vec3(0.0f);
            glm::vec3 m_Scale = glm::vec3(1.0f);

            glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
            glm::vec3 m_Forward = glm::vec3(0.0f, 0.0f, -1.0f);

            bool m_Dirty = true;

            Shared<Transform> m_Parent = nullptr;
        };
    }
}
