#include "precompiled.h"

#include "Renderer/Buffer.h"

#include "glad/gl.h"

namespace SmartGL
{

    // VertexBuffer

    VertexBuffer::VertexBuffer(uint32_t size)
    {
        glCreateBuffers(1, &m_RendererID);
        glNamedBufferStorage(m_RendererID, size, nullptr, GL_DYNAMIC_STORAGE_BIT);
    }

    VertexBuffer::VertexBuffer(uint32_t size, const void *vertices)
    {
        glCreateBuffers(1, &m_RendererID);
        glNamedBufferStorage(m_RendererID, size, vertices, GL_DYNAMIC_STORAGE_BIT);
    }

    VertexBuffer::~VertexBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    void VertexBuffer::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }

    void VertexBuffer::Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::SetData(uint32_t size, const void *data)
    {
        glNamedBufferSubData(m_RendererID, 0, size, data);
    }

    // IndexBuffer

    IndexBuffer::IndexBuffer(uint32_t count)
        : m_Count(count)
    {
        glCreateBuffers(1, &m_RendererID);
        glNamedBufferStorage(m_RendererID, count * sizeof(uint32_t), nullptr, GL_DYNAMIC_STORAGE_BIT);
    }

    IndexBuffer::IndexBuffer(uint32_t *indices, uint32_t count)
        : m_Count(count)
    {
        glCreateBuffers(1, &m_RendererID);
        glNamedBufferStorage(m_RendererID, count * sizeof(uint32_t), indices, GL_DYNAMIC_STORAGE_BIT);
    }

    IndexBuffer::~IndexBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    void IndexBuffer::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }

    void IndexBuffer::Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void IndexBuffer::SetData(uint32_t count, const void *data)
    {
        m_Count = count;
        glNamedBufferSubData(m_RendererID, 0, count * sizeof(uint32_t), data);
    }
}
