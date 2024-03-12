#include "precompiled.h"

#include "Renderer/RenderPass.h"
#include "Renderer/RenderCommand.h"

#include "Core/Log.h"

namespace SmartGL
{
    RenderPass::RenderPass(RenderPassSpecifications spec)
        : m_Specifications(spec)
    {
        m_Framebuffer = CreateShared<FrameBuffer>(m_Specifications.FrameBufferSpecs);
    }

    RenderPass::~RenderPass()
    {
        m_Specifications.Program.reset();
        m_Framebuffer.reset();
    }

    void RenderPass::BeginPass()
    {
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor(m_Specifications.ClearColor);
        RenderCommand::Clear({RenderBuffer::Color, RenderBuffer::Depth});
    }

    void RenderPass::EndPass()
    {
        m_Framebuffer->Unbind();
    }

    void RenderPass::Blit(uint32_t attachementIndex /*= 0*/)
    {
        m_Framebuffer->BlitTo(m_Specifications.RenderTarget, attachementIndex);
    }

    void RenderPass::Resize(uint32_t width, uint32_t height)
    {
        m_Framebuffer->Resize(width, height);
    }

    void RenderPass::ClearTexture(uint32_t textureID, int clearID)
    {
        m_Framebuffer->ClearTextureAttachment(textureID, &clearID);
    }

    void RenderPass::SetSpecifications(const RenderPassSpecifications &spec)
    {
        m_Specifications = spec;
        if(m_Framebuffer != nullptr)
            m_Framebuffer->SetSpecifications(m_Specifications.FrameBufferSpecs);
        else
            m_Framebuffer = CreateShared<FrameBuffer>(m_Specifications.FrameBufferSpecs);
    }

    void RenderPass::BindShader()
    {
        if(m_Specifications.Program == nullptr)
            SMART_LOG_WARN("RenderPass::BindShader: No shader program set for this render pass");
        else
            m_Specifications.Program->Bind();
    }
}
