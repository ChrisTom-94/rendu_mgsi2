#include "precompiled.h"

#include "ShaderStorageBuffer.h"

#include "glad/gl.h"

namespace SmartGL
{
    ShaderStorageBuffer::ShaderStorageBuffer(uint32_t size, const void *data)
    {
        glCreateBuffers(1, &m_RendererID);
        glNamedBufferStorage(m_RendererID, size, data, GL_DYNAMIC_STORAGE_BIT);
    }

    ShaderStorageBuffer::~ShaderStorageBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    void ShaderStorageBuffer::SetData(const void *data, uint32_t size, uint32_t offset)
    {
        glNamedBufferSubData(m_RendererID, offset, size, data);
    }

    void ShaderStorageBuffer::Bind(uint32_t binding)
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, m_RendererID);
    }

    void ShaderStorageBuffer::Unbind()
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
}
