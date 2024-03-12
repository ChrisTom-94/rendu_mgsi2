#pragma once

#include "Renderer/Shader.h"

namespace SmartGL
{
    struct BufferElement
    {
        std::string Name;
        uint32_t Size;
        uint32_t Offset;
        ShaderDataType Type;
        bool Normalized;

        BufferElement() = default;

        BufferElement(ShaderDataType type, const std::string name, bool normalized = false)
            : Name(name), Size(ShaderDataTypeSize(type)), Type(type), Normalized(normalized), Offset(0)
        {
        }

        uint32_t GetComponentCount() const
        {
            switch (static_cast<ShaderDataType>(Type))
            {
            case ShaderDataType::Float:
                return 1;
            case ShaderDataType::Float2:
                return 2;
            case ShaderDataType::Float3:
                return 3;
            case ShaderDataType::Float4:
                return 4;
            case ShaderDataType::Mat3:
                return 3 * static_cast<uint32_t>(ShaderDataType::Float3);
            case ShaderDataType::Mat4:
                return 4 * static_cast<uint32_t>(ShaderDataType::Float3);
            case ShaderDataType::Int:
                return 1;
            case ShaderDataType::Int2:
                return 2;
            case ShaderDataType::Int3:
                return 3;
            case ShaderDataType::Int4:
                return 4;
            case ShaderDataType::Bool:
                return 1;
            default:
                SMART_ASSERT(false, "Unknown ShaderDataType!");
                return 0;
            }
        }
    };

    class BufferLayout
    {
    public:
        BufferLayout() : m_Stride(0){};
        BufferLayout(const std::initializer_list<BufferElement> &elements)
            : m_Elements(elements)
        {
            ComputeOffsetsAndStride();
        }

        inline const std::vector<BufferElement> &GetElements() const { return m_Elements; }
        inline uint32_t GetStride() const { return m_Stride; }

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

    private:
        void ComputeOffsetsAndStride()
        {
            uint32_t offset = 0;
            m_Stride = 0;
            for (auto &element : m_Elements)
            {
                element.Offset = offset;
                offset += element.Size;
                m_Stride += element.Size;
            }
        }

    private:
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0;
    };

    class VertexBuffer
    {
    public:
        VertexBuffer(uint32_t size);
        VertexBuffer(uint32_t size, const void *vertices);
        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;

        const BufferLayout &GetLayout() const { return m_Layout; }
        uint32_t GetRendererID() const { return m_RendererID; }

        void SetData(uint32_t size, const void *data);
        void SetLayout(const BufferLayout &layout) { m_Layout = layout; }

    private:
        uint32_t m_RendererID;
        BufferLayout m_Layout;
    };

    class IndexBuffer
    {
    public:
        IndexBuffer(uint32_t count);
        IndexBuffer(uint32_t *indices, uint32_t count);
        ~IndexBuffer();

        void Bind() const;
        void Unbind() const;

        uint32_t GetCount() const { return m_Count; }
        uint32_t GetRendererID() const { return m_RendererID; }

        void SetData(uint32_t count, const void *data);

    private:
        uint32_t m_RendererID;
        uint32_t m_Count;
    };
}
