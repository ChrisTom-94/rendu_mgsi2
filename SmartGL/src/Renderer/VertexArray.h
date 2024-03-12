#pragma once

#include "Core/TypesDefinitions.h"
#include "Renderer/Buffer.h"

namespace SmartGL
{
    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        void Bind() const;
        void Unbind() const;

        void AddVertexBuffer(const Shared<VertexBuffer> &vertexBuffer);
        void AddVertexBuffers(const std::vector<Shared<VertexBuffer>> &vertexBuffers);
        void SetIndexBuffer(const Shared<IndexBuffer> &indexBuffer);
        void Clear();

        const std::vector<Shared<VertexBuffer>> &GetVertexBuffers() const { return m_VertexBuffers; }
        const Shared<IndexBuffer> &GetIndexBuffer() const { return m_IndexBuffer; }
        const uint32_t GetRendererID() const { return m_RendererID; }


    private:

        uint32_t m_RendererID;
        uint32_t m_VertexBufferIndex = 0;
        std::vector<Shared<VertexBuffer>> m_VertexBuffers;
        std::vector<uint32_t> m_VertexBuffersIDs;
        std::vector<int64_t> m_VertexBuffersOffsets;
        std::vector<int32_t> m_VertexBuffersStrides;
        Shared<IndexBuffer> m_IndexBuffer;
    };
}
