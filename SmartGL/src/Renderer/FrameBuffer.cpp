#include "precompiled.h"

#include "Renderer/FrameBuffer.h"
#include "Renderer/Texture.h"

#include "Core/Log.h"

#include "glad/gl.h"

namespace SmartGL
{
    static int FrameBufferBindTypeToOpenGL(FrameBufferBind type)
    {
        switch (type)
        {
        case FrameBufferBind::Read:
            return GL_READ_FRAMEBUFFER;
        case FrameBufferBind::Write:
            return GL_DRAW_FRAMEBUFFER;
        case FrameBufferBind::Both:
            return GL_FRAMEBUFFER;
        }

        SMART_LOG_CRITICAL("Unknown FrameBufferBind!");
        SMART_BREAK();
        return 0;
    }

    static uint32_t FrameBufferTextureInternalFormatToOpenGL(FrameBufferAttachmentFormat format)
    {
        switch (format)
        {
        case FrameBufferAttachmentFormat::RGBA8:
            return GL_RGBA8;
        case FrameBufferAttachmentFormat::RED_INTEGER:
            return GL_R32I;
        case FrameBufferAttachmentFormat::RGBA16F:
            return GL_RGBA16F;
        case FrameBufferAttachmentFormat::DEPTH24STENCIL8:
            return GL_DEPTH24_STENCIL8;
        case FrameBufferAttachmentFormat::DEPTH32F:
            return GL_DEPTH_COMPONENT32F;
        }

        SMART_LOG_CRITICAL("Unknown FrameBufferAttachmentFormat!");
        SMART_BREAK();
        return 0;
    }

    static uint32_t FrameBufferTextureFormatToOpenGL(FrameBufferAttachmentFormat format)
    {
        switch (format)
        {
        case FrameBufferAttachmentFormat::RGBA8:
            return GL_RGBA;
        case FrameBufferAttachmentFormat::RED_INTEGER:
            return GL_RED_INTEGER;
        case FrameBufferAttachmentFormat::RGBA16F:
            return GL_RGBA;
        case FrameBufferAttachmentFormat::DEPTH24STENCIL8:
            return GL_DEPTH_STENCIL;
        case FrameBufferAttachmentFormat::DEPTH32F:
            return GL_DEPTH_COMPONENT;
        }

        SMART_LOG_CRITICAL("Unknown FrameBufferAttachmentFormat!");
        SMART_BREAK();
        return 0;
    }

    static uint32_t FrameBufferTextureDataTypeToOpenGL(FrameBufferTextureDataType type)
    {
        switch (type)
        {
        case FrameBufferTextureDataType::UnsignedByte:
            return GL_UNSIGNED_BYTE;
        case FrameBufferTextureDataType::Float:
            return GL_FLOAT;
        }

        SMART_LOG_CRITICAL("Unknown FrameBufferTextureDataType!");
        SMART_BREAK();
        return 0;
    }

    FrameBuffer::FrameBuffer(const FrameBufferSpecifications &specifications)
        : m_Specifications(specifications)
    {
        SetSpecifications(specifications);
    }

    FrameBuffer::~FrameBuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(1, m_TextureAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);
    }

    void FrameBuffer::Bind(FrameBufferBind bind)
    {
        GLenum bindType = FrameBufferBindTypeToOpenGL(bind);
        glBindFramebuffer((GLenum)bindType, m_RendererID);
        glViewport(0, 0, m_Specifications.Width, m_Specifications.Height);
    }

    void FrameBuffer::Unbind(FrameBufferBind bind)
    {
        GLenum bindType = FrameBufferBindTypeToOpenGL(bind);
        glBindFramebuffer(bindType, 0);
    }

    void FrameBuffer::BlitTo(const uint32_t &frameBuffer, uint32_t attachmentIndex)
    {
        Bind(FrameBufferBind::Read);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        glBlitFramebuffer(0, 0, m_Specifications.Width, m_Specifications.Height, 0, 0, m_Specifications.Width, m_Specifications.Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        Unbind(FrameBufferBind::Read);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    void FrameBuffer::BlitTo(const Shared<FrameBuffer> &frameBuffer, uint32_t attachmentIndex)
    {

        Bind(FrameBufferBind::Read);
        frameBuffer->Bind(FrameBufferBind::Write);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        glBlitFramebuffer(0, 0, m_Specifications.Width, m_Specifications.Height, 0, 0, frameBuffer->m_Specifications.Width, frameBuffer->m_Specifications.Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        Unbind(FrameBufferBind::Read);
        frameBuffer->Unbind(FrameBufferBind::Write);
    }

    void FrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        m_Specifications.Width = width;
        m_Specifications.Height = height;

        UpdateAttachments();
    }

    void FrameBuffer::SetSpecifications(const FrameBufferSpecifications &specifications)
    {
        m_Specifications = specifications;
        for (auto specification : m_Specifications.Attachments)
        {
            if (!(specification.TextureFormat == FrameBufferAttachmentFormat::Depth))
                m_TextureAttachmentSpecifications.emplace_back(specification);
            else
                m_DepthAttachmentSpecifications = specification;
        }
        UpdateAttachments();
    }

    void FrameBuffer::UpdateAttachments()
    {

        if (m_RendererID && m_TextureAttachments.size())
            Delete();

        glCreateFramebuffers(1, &m_RendererID);

        bool multisample = m_Specifications.Samples > 1;

        bool hasColorAttachments = m_TextureAttachmentSpecifications.size() > 0;
        if (hasColorAttachments)
            AddTextureAttachments();

        bool hasDepthAttachment = m_DepthAttachmentSpecifications.TextureFormat != FrameBufferAttachmentFormat::None;
        if (hasDepthAttachment)
            AddDepthAttachment();

        if (m_TextureAttachments.size() > 1)
            DrawTextureAttachments();
        else if (m_TextureAttachments.empty())
            glNamedFramebufferDrawBuffer(m_RendererID, GL_NONE);

        unsigned int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        SMART_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete : " + &status);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::AddTextureAttachments()
    {

        bool multisample = m_Specifications.Samples > 1;

        m_TextureAttachments.resize(m_TextureAttachmentSpecifications.size());

        glGenTextures(m_TextureAttachmentSpecifications.size(), m_TextureAttachments.data());

        for (uint32_t i = 0; i < m_TextureAttachments.size(); i++)
        {
            glBindTexture(TextureUtils::TextureTarget(multisample), m_TextureAttachments[i]);

            FrameBufferTextureSpecifications &specification = m_TextureAttachmentSpecifications[i];
            uint32_t samples = m_Specifications.Samples;
            GLenum internalFormat = FrameBufferTextureInternalFormatToOpenGL(specification.TextureFormat);
            GLenum format = FrameBufferTextureFormatToOpenGL(specification.TextureFormat);
            GLenum dataType = FrameBufferTextureDataTypeToOpenGL(specification.TextureDataType);
            uint32_t width = m_Specifications.Width;
            uint32_t height = m_Specifications.Height;

            bool multisampled = samples > 1;
            if (multisampled)
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
            else
            {
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, nullptr);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                // if (format =! GL_FLOAT){
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                // }
            }

            glNamedFramebufferTexture(m_RendererID, GL_COLOR_ATTACHMENT0 + i, m_TextureAttachments[i], 0);
        }
    }

    void FrameBuffer::AddDepthAttachment()
    {
        glCreateRenderbuffers(1, &m_DepthAttachment);

        uint32_t samples = m_Specifications.Samples;
        uint32_t width = m_Specifications.Width;
        uint32_t height = m_Specifications.Height;
        GLenum internalFormat = FrameBufferTextureInternalFormatToOpenGL(m_DepthAttachmentSpecifications.TextureFormat);

        GLenum attachementType;
        if(m_DepthAttachmentSpecifications.TextureFormat == FrameBufferAttachmentFormat::DEPTH24STENCIL8)
            attachementType = GL_DEPTH_STENCIL_ATTACHMENT;
        else
            attachementType = GL_DEPTH_ATTACHMENT;

        bool multisampled = samples > 1;
        if (multisampled)
            glNamedRenderbufferStorageMultisample(m_DepthAttachment, samples, internalFormat, width, height);
        else
            glNamedRenderbufferStorage(m_DepthAttachment, internalFormat, width, height);

        glNamedFramebufferRenderbuffer(m_RendererID, attachementType, GL_RENDERBUFFER, m_DepthAttachment);
    }

    void FrameBuffer::DrawTextureAttachment(uint32_t index)
    {
        SMART_ASSERT(index < m_TextureAttachments.size(), "Index out of range!");

        GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0 + index};
        glDrawBuffers(1, drawBuffers);
    }

    void FrameBuffer::DrawTextureAttachments()
    {
        SMART_ASSERT(m_TextureAttachments.size() <= 4, "Too many texture Attachments!");

        GLenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};

        glNamedFramebufferDrawBuffers(m_RendererID, 4, buffers);
    }

    void FrameBuffer::ReadTextureAttachment(uint32_t index)
    {
        SMART_ASSERT(index < m_TextureAttachments.size(), "Index out of range!");
        glNamedFramebufferReadBuffer(m_RendererID, GL_COLOR_ATTACHMENT0 + index);
    }

    void FrameBuffer::ReadTextureUnitAttachment(uint32_t index, uint32_t slot)
    {
        SMART_ASSERT(index < m_TextureAttachments.size(), "Index out of range!");
        TextureUtils::BindTextureUnit(m_TextureAttachments[index], slot);
    }

    void FrameBuffer::ClearTextureAttachment(uint32_t index, const void *value)
    {
        SMART_ASSERT(index < m_TextureAttachments.size(), "Index out of range!");
        auto format = FrameBufferTextureFormatToOpenGL(m_TextureAttachmentSpecifications[index].TextureFormat);
        auto dataType = FrameBufferTextureDataTypeToOpenGL(m_TextureAttachmentSpecifications[index].TextureDataType);
        TextureUtils::ClearTexture(m_TextureAttachments[index], format, dataType, value);
    }

    void FrameBuffer::Delete()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(1, m_TextureAttachments.data());
        glDeleteRenderbuffers(1, &m_DepthAttachment);

        m_TextureAttachments.clear();
        m_DepthAttachment = 0;
    }

    int FrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
    {
        SMART_ASSERT(attachmentIndex < m_TextureAttachments.size(), "Index out of range!");

        SMART_ASSERT(
            m_TextureAttachmentSpecifications[attachmentIndex].TextureFormat == FrameBufferAttachmentFormat::RED_INTEGER,
            "Texture is not RED_INTEGER!");

        FrameBufferTextureSpecifications &specification = m_TextureAttachmentSpecifications[attachmentIndex];
        GLenum format = FrameBufferTextureFormatToOpenGL(specification.TextureFormat);

        ReadTextureAttachment(attachmentIndex);
        int value;
        glReadPixels(x, y, 1, 1, format, GL_INT, &value);
        return value;
    }
}
