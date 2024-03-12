#include "precompiled.h"

#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer2D.h"

namespace SmartGL
{
    Unique<Renderer::RendererData> Renderer::s_RendererData = CreateUnique<Renderer::RendererData>();

    void Renderer::Init()
    {
        RenderCommand::Init();
        Renderer2D::Init();
    }

    void Renderer::Shutdown()
    {
        Renderer2D::Shutdown();
    }

    void Renderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0, 0, width, height);
    }

    void Renderer::BeginScene(Camera &camera)
    {
        s_RendererData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
    }

    void Renderer::EndScene()
    {
    }
}
