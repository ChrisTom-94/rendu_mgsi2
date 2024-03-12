#pragma once

#include "Core/TypesDefinitions.h"

#include <vector>


namespace SmartGL
{

    enum class FrameBufferBind
    {
        Read = 0,
        Write = 1,
        Both = 2
    };

    enum class FrameBufferAttachmentFormat
    {
        None = 0,

        // Color
        RED_INTEGER,
        RGBA8,
        RGBA16F,

        // Depth/stencil
        DEPTH24STENCIL8,
        DEPTH32F,

        // Defaults
        Depth = DEPTH32F
    };

    enum class FrameBufferTextureDataType
    {
        None = 0,
        UnsignedByte,
        Float
    };

    struct FrameBufferTextureSpecifications
    {
        FrameBufferTextureSpecifications() = default;
        FrameBufferTextureSpecifications(FrameBufferAttachmentFormat format, FrameBufferTextureDataType dataType = FrameBufferTextureDataType::None)
            : TextureFormat(format), TextureDataType(dataType) {}

        FrameBufferAttachmentFormat TextureFormat = FrameBufferAttachmentFormat::None;
        FrameBufferTextureDataType TextureDataType = FrameBufferTextureDataType::None;
    };

    struct FrameBufferSpecifications
    {
        uint32_t Width, Height;
        uint32_t Samples = 1;
        std::vector<FrameBufferTextureSpecifications> Attachments;

        bool SwapChainTarget = false;
    };

    class FrameBuffer
    {

    public:
        FrameBuffer() = default;
        FrameBuffer(const FrameBufferSpecifications &specifications);
        ~FrameBuffer();

        void Bind(FrameBufferBind bind = FrameBufferBind::Both);
        void Unbind(FrameBufferBind bind = FrameBufferBind::Both);
        void BlitTo(const uint32_t &frameBuffer = 0, uint32_t attachmentIndex = 0);
        void BlitTo(const Shared<FrameBuffer> &frameBuffer = nullptr, uint32_t attachmentIndex = 0);

        void SetSpecifications(const FrameBufferSpecifications &specifications);
        void UpdateAttachments();

        void Delete();
        void Resize(uint32_t width, uint32_t height);
        int ReadPixel(uint32_t attachmentIndex, int x, int y);

        void ReadTextureAttachment(uint32_t index);
        void ReadTextureUnitAttachment(uint32_t index, uint32_t slot);

        void DrawTextureAttachment(uint32_t index = 0);
        void DrawTextureAttachments();

        void ClearTextureAttachment(uint32_t index, const void *value);

        inline uint32_t GetID() const { return m_RendererID; };
        inline uint32_t GetTextureAttachmentID(uint32_t index) { return m_TextureAttachments[index]; };
        inline std::vector<uint32_t> GetTextureAttachmentIDs() { return m_TextureAttachments; };
        inline uint32_t GetDepthAttachmentRendererID() { return m_DepthAttachment; };
        inline uint32_t GetWidth() const { return m_Specifications.Width; };
        inline uint32_t GetHeight() const { return m_Specifications.Height; };

    private:
        void AddTextureAttachments();
        void AddDepthAttachment();

    private:
        uint32_t m_RendererID;
        FrameBufferSpecifications m_Specifications;

        std::vector<FrameBufferTextureSpecifications> m_TextureAttachmentSpecifications;
        FrameBufferTextureSpecifications m_DepthAttachmentSpecifications = FrameBufferAttachmentFormat::None;

        std::vector<uint32_t> m_TextureAttachments;
        uint32_t m_DepthAttachment;
    };

}
