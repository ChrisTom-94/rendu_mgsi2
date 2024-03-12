#pragma once

#include "Core/Assert.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "glad/gl.h"

#include <string>
#include <unordered_map>

namespace SmartGL
{
    enum class ShaderType
    {
        NONE = 0,
        VERTEX = GL_VERTEX_SHADER,
        FRAGMENT = GL_FRAGMENT_SHADER
    };

    struct ShaderSources
    {
        std::string VertexSource;
        std::string FragmentSource;
    };

    static GLenum ShaderTypeToGL(ShaderType type)
    {
        switch (type)
        {
        case ShaderType::VERTEX:
            return GL_VERTEX_SHADER;
        case ShaderType::FRAGMENT:
            return GL_FRAGMENT_SHADER;
        default:
            SMART_ASSERT(false, "Unknown ShaderType!");
        }
        return 0;
    }

    class Shader
    {
    public:
        Shader(const std::string &name, const std::string &filepath);
        Shader(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc);
        ~Shader();

        void Bind() const;
        void Unbind() const;

        void SetInt(const std::string &name, int value);
        void SetIntArray(const std::string &name, int *values, uint32_t count);
        void SetFloat(const std::string &name, float value);
        void SetFloat2(const std::string &name, const glm::vec2 &value);
        void SetFloat3(const std::string &name, const glm::vec3 &value);
        void SetFloat4(const std::string &name, const glm::vec4 &value);
        void SetMat3(const std::string &name, const glm::mat3 &matrix);
        void SetMat4(const std::string &name, const glm::mat4 &matrix);

        const std::string &GetName() const { return m_Name; }

        void UploadUniformInt(const std::string &name, int value);
        void UploadUniformIntArray(const std::string &name, int *values, uint32_t count);
        void UploadUniformFloat(const std::string &name, float value);
        void UploadUniformFloat2(const std::string &name, const glm::vec2 &value);
        void UploadUniformFloat3(const std::string &name, const glm::vec3 &value);
        void UploadUniformFloat4(const std::string &name, const glm::vec4 &value);
        void UploadUniformMat3(const std::string &name, const glm::mat3 &matrix);
        void UploadUniformMat4(const std::string &name, const glm::mat4 &matrix);

    private:
        ShaderSources Parse(const std::string &filepath);
        ShaderSources ParseGLSL(const std::string &filepath);
        std::string ReadFile(const std::string &filepath);
        uint32_t Compile(ShaderType type, const std::string &source);
        uint32_t Link(const std::string &vertexSrc, const std::string &fragmentSrc);
        GLint GetUniformLocation(const std::string &name) const;

    private:
        uint32_t m_ProgramID;
        std::string m_Name;
        std::unordered_map<GLenum, std::string> m_ShaderSource;
        mutable std::unordered_map<std::string, int> m_UniformLocationCache;
    };

    enum class ShaderDataType
    {
        None = 0,
        Float,
        Float2,
        Float3,
        Float4,
        Mat3,
        Mat4,
        Int,
        Int2,
        Int3,
        Int4,
        Bool
    };

    static uint32_t ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float:
            return 4;
        case ShaderDataType::Float2:
            return 4 * 2;
        case ShaderDataType::Float3:
            return 4 * 3;
        case ShaderDataType::Float4:
            return 4 * 4;
        case ShaderDataType::Mat3:
            return 4 * 3 * 3;
        case ShaderDataType::Mat4:
            return 4 * 4 * 4;
        case ShaderDataType::Int:
            return 4;
        case ShaderDataType::Int2:
            return 4 * 2;
        case ShaderDataType::Int3:
            return 4 * 3;
        case ShaderDataType::Int4:
            return 4 * 4;
        case ShaderDataType::Bool:
            return 1;
        default:
            SMART_ASSERT(false, "Unknown ShaderDataType!");
        }

        return 0;
    }
}
