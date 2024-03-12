#include "precompiled.h"

#include "Renderer/VertexArray.h"

#include "glad/gl.h"

namespace SmartGL
{
    static GLenum ShaderDataTypeToOpenGLType(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float:
            return GL_FLOAT;
        case ShaderDataType::Float2:
            return GL_FLOAT;
        case ShaderDataType::Float3:
            return GL_FLOAT;
        case ShaderDataType::Float4:
            return GL_FLOAT;
        case ShaderDataType::Mat3:
            return GL_FLOAT;
        case ShaderDataType::Mat4:
            return GL_FLOAT;
        case ShaderDataType::Int:
            return GL_INT;
        case ShaderDataType::Int2:
            return GL_INT;
        case ShaderDataType::Int3:
            return GL_INT;
        case ShaderDataType::Int4:
            return GL_INT;
        case ShaderDataType::Bool:
            return GL_BOOL;
        default:
            SMART_ASSERT(false, "Unknown ShaderDataType!");
            break;
        };

        return 0;
    }

    VertexArray::VertexArray()
    {
        glCreateVertexArrays(1, &m_RendererID);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void VertexArray::Bind() const
    {
        glBindVertexArray(m_RendererID);
    }

    void VertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void VertexArray::AddVertexBuffer(const Shared<VertexBuffer> &vertexBuffer)
    {

        const auto &layout = vertexBuffer->GetLayout();

        glVertexArrayVertexBuffer(
            m_RendererID,
            m_VertexBufferIndex,
            vertexBuffer->GetRendererID(),
            0,
            layout.GetStride());

        bool isPacked = layout.GetElements().size() > 1;

        auto index = 0;
        for (const auto &element : layout)
        {
            if(isPacked)
                glEnableVertexArrayAttrib(m_RendererID, index);
            else
                glEnableVertexArrayAttrib(m_RendererID, m_VertexBufferIndex);

            switch (element.Type)
            {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:
            {
                glVertexArrayAttribFormat(
                    m_RendererID,
                    isPacked ? index : m_VertexBufferIndex,
                    element.GetComponentCount(),
                    ShaderDataTypeToOpenGLType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    element.Offset);
                break;
            }
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Bool:
            {
                glVertexArrayAttribIFormat(
                    m_RendererID,
                    isPacked ? index : m_VertexBufferIndex,
                    element.GetComponentCount(),
                    ShaderDataTypeToOpenGLType(element.Type),
                    element.Offset);
                break;
            }
            default:
                SMART_ASSERT(false, "Unknown ShaderDataType!");
                break;
            }
            glVertexArrayAttribBinding(m_RendererID, isPacked ? index : m_VertexBufferIndex, m_VertexBufferIndex);
            index++;
        }
        m_VertexBufferIndex++;
        m_VertexBuffers.push_back(vertexBuffer);
    }

    void VertexArray::AddVertexBuffers(const std::vector<Shared<VertexBuffer>> &vertexBuffers)
    {
        for (const auto &vertexBuffer : vertexBuffers)
            AddVertexBuffer(vertexBuffer);
    }

    void VertexArray::SetIndexBuffer(const Shared<IndexBuffer> &indexBuffer)
    {
        m_IndexBuffer = indexBuffer;
        glVertexArrayElementBuffer(m_RendererID, m_IndexBuffer->GetRendererID());
    }

    void VertexArray::Clear()
    {
        m_VertexBufferIndex = 0;
        m_VertexBuffers.clear();
        m_IndexBuffer.reset();
    }
}
