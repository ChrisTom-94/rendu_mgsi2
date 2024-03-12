#include "precompiled.h"

#include "Renderer/UniformBuffer.h"

#include "glad/gl.h"

namespace SmartGL
{
    UniformBuffer::UniformBuffer(uint32_t size)
    {
        glCreateBuffers(1, &m_RendererID);
        glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
    }

    UniformBuffer::~UniformBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    void UniformBuffer::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        glNamedBufferSubData(m_RendererID, offset, size, data);
    }

    void UniformBuffer::Bind(uint16_t binding) const
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
    }

    void UniformBuffer::Unbind() const
    {
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}
