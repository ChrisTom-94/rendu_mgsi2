#pragma once

namespace SmartGL
{
    class UniformBuffer
    {
    public:
        UniformBuffer(uint32_t size);
        ~UniformBuffer();

        void SetData(const void* data, uint32_t size, uint32_t offset = 0);
        void Bind(uint16_t binding) const;
        void Unbind() const;

    private:
        uint32_t m_RendererID = 0;
    };
}
