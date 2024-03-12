#include "precompiled.h"

#include "Renderer/GBuffer.h"

#include "Core/Log.h"

namespace SmartGL
{
    GBuffer::GBuffer()
    {
    }

    GBuffer::~GBuffer()
    {
    }

    GBuffer::GBuffer(uint32_t width, uint32_t height)
    {
        Init(width, height);
    }

    void GBuffer::Init(uint32_t width, uint32_t height)
    {
        FrameBufferSpecifications fboSpecs;
        fboSpecs.Width = width;
        fboSpecs.Height = height;
        fboSpecs.Attachments = {
            {FrameBufferAttachmentFormat::RGBA16F, FrameBufferTextureDataType::Float}, // position
            {FrameBufferAttachmentFormat::RGBA16F, FrameBufferTextureDataType::Float}, // normal
            {FrameBufferAttachmentFormat::RGBA8, FrameBufferTextureDataType::UnsignedByte}, // albedo + shininess
            {FrameBufferAttachmentFormat::Depth}};

        m_FrameBuffer = CreateShared<FrameBuffer>(fboSpecs);
        Clear();

#if defined(SMART_PLATFORM_LINUX) || defined(SMART_COMPILER_MINGW)
        std::string shaderPath = "SmartGL/assets/shaders/g-buffer.glsl";
#else
        std::string shaderPath = "../SmartGL/assets/shaders/g-buffer.glsl";
#endif

        m_Shader = CreateShared<Shader>("GBuffer", shaderPath);
    }

    void GBuffer::Bind()
    {
        m_Shader->Bind();
        m_FrameBuffer->Bind(FrameBufferBind::Write);
    }

    void GBuffer::BindTextures()
    {
        m_FrameBuffer->ReadTextureUnitAttachment(0, 0);
        m_FrameBuffer->ReadTextureUnitAttachment(1, 1);
        m_FrameBuffer->ReadTextureUnitAttachment(2, 2);
    }

    void GBuffer::Unbind()
    {
        m_Shader->Unbind();
        m_FrameBuffer->Unbind(FrameBufferBind::Write);
    }

    void GBuffer::Clear()
    {
        glm::vec4 black = {0.0f, 0.0f, 0.0f, 0.0f};
        m_FrameBuffer->ClearTextureAttachment(0, &black);
        m_FrameBuffer->ClearTextureAttachment(1, &black);
        m_FrameBuffer->ClearTextureAttachment(2, &black);
    }

    void GBuffer::Resize(uint32_t width, uint32_t height)
    {
        m_FrameBuffer->Resize(width, height);
    }
}
