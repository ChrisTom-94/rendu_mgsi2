#pragma once

#include <string>
#include <cstdint>

namespace SmartGL
{
    class ShaderStorageBuffer
    {
    public:
        ShaderStorageBuffer(uint32_t size, const void *data = nullptr);
        ~ShaderStorageBuffer();

        void SetData(const void *data, uint32_t size, uint32_t offset = 0);
        void Bind(uint32_t binding);
        void Unbind();

    private:
        uint32_t m_RendererID;
    };
}
