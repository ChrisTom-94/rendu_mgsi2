#include "Renderer.h"

namespace TP2_Nurbs
{
    struct SurfaceBuffers
    {
        const uint32_t MaxVertices = 10000;
        const uint32_t MaxIndices = 10000 * 6;

        Shared<VertexArray> VAO;
        Shared<VertexBuffer> VBO;
        Shared<IndexBuffer> IBO;
        Shared<Shader> Program;
    };

    static SurfaceBuffers s_SurfaceBuffers;
    static RenderPass s_RenderPass;
    static bool s_isWireframe = false;

    void Renderer::Init()
    {
        float width = Core::Application::Get().GetWindow().GetWidth();
        float height = Core::Application::Get().GetWindow().GetHeight();

        RenderCommand::SetViewport(0, 0, width, height);
        RenderCommand::SetLineWidth(2.0f);

        // setup renderpass
        RenderPassSpecifications renderPassSpecs;
        renderPassSpecs.FrameBufferSpecs.Width = width;
        renderPassSpecs.FrameBufferSpecs.Height = height;
        renderPassSpecs.FrameBufferSpecs.Attachments = {
            {FrameBufferAttachmentFormat::RGBA8, FrameBufferTextureDataType::UnsignedByte},       // color
            {FrameBufferAttachmentFormat::RED_INTEGER, FrameBufferTextureDataType::UnsignedByte}, // picking
            {FrameBufferAttachmentFormat::Depth}
        };
        renderPassSpecs.ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};

        std::string workingDirectory = Core::Application::Get().GetSpecification().WorkingDirectory;
        std::string shaderPath = workingDirectory + "shaders/shader.glsl";
        renderPassSpecs.Program = CreateShared<Shader>("TextureQuadShader", shaderPath);

        s_RenderPass.SetSpecifications(renderPassSpecs);

        // setup surface buffers
        s_SurfaceBuffers.VAO = CreateShared<VertexArray>();

        s_SurfaceBuffers.VBO = CreateShared<VertexBuffer>(s_SurfaceBuffers.MaxVertices * sizeof(glm::vec4));
        BufferLayout layout = {
            {ShaderDataType::Float4, "a_PositionAndCurvature"},
        };
        s_SurfaceBuffers.VBO->SetLayout(layout);
        s_SurfaceBuffers.VAO->AddVertexBuffer(s_SurfaceBuffers.VBO);

        s_SurfaceBuffers.IBO = CreateShared<IndexBuffer>(s_SurfaceBuffers.MaxIndices);
        s_SurfaceBuffers.VAO->SetIndexBuffer(s_SurfaceBuffers.IBO);

        shaderPath = workingDirectory + "shaders/surface.glsl";
        s_SurfaceBuffers.Program = CreateShared<Shader>("Suface Shader", shaderPath);
    }

    void Renderer::ShutDown()
    {}

    void Renderer::DrawControlPoints(const std::vector<std::vector<ControlPoint>> &controlPoints)
    {
        glm::vec4 pointColor;
        glm::vec4 lineUColor = {0.7f, 1.0f, 0.0f, 0.5f};
        glm::vec4 lineVColor = {0.0f, 1.0f, 0.7f, 0.5f};
        float radius = 0.1f;
        float thickness = 1.0f;
        float fade = 0.005f;

        for (uint32_t u = 0; u < controlPoints.size(); u++)
            for (uint32_t v = 0; v < controlPoints[u].size(); v++)
            {
                const ControlPoint &point = controlPoints[u][v];

                // draw control points
                pointColor = point.Selected || point.Hovered ? point.SelectedColor : point.Color;
                Renderer2D::DrawCircle(point.Position, pointColor, radius, thickness, fade, point.ID);

                // draw lines
                if (u > 0)
                    Renderer2D::DrawLine(controlPoints[u - 1][v].Position, point.Position, lineUColor);
                if (v > 0)
                    Renderer2D::DrawLine(controlPoints[u][v - 1].Position, point.Position, lineVColor);

            }
    }

    void Renderer::DrawSurface(std::vector<std::vector<glm::vec3>> &points, std::vector<std::vector<float>> curvatures, bool showCurvatureMap)
    {
        std::vector<float> vertices;

        for(int i = 0; i < points.size(); i++)
            for(int j = 0; j < points[i].size(); j++)
            {
                glm::vec3 point = points[i][j];
                vertices.push_back(point.x);
                vertices.push_back(point.y);
                vertices.push_back(point.z);

                if(curvatures.size() == 0)
                {
                    vertices.push_back(1.0f);
                    continue;
                }

                float curvature = curvatures[i][j];
                vertices.push_back(curvature);
            }

        uint32_t bufferSize = vertices.size() * sizeof(float);
        s_SurfaceBuffers.VBO->SetData(bufferSize, vertices.data());

        std::vector<uint32_t> indices;
        for (uint32_t i = 0; i < points.size() - 1; i++)
            for (uint32_t j = 0; j < points[i].size() - 1; j++)
            {
                indices.push_back(i * points[i].size() + j);
                indices.push_back((i + 1) * points[i].size() + j);
                indices.push_back((i + 1) * points[i].size() + j + 1);

                indices.push_back(i * points[i].size() + j);
                indices.push_back((i + 1) * points[i].size() + j + 1);
                indices.push_back(i * points[i].size() + j + 1);
            }

        s_SurfaceBuffers.IBO->SetData(indices.size(), indices.data());

        s_SurfaceBuffers.Program->Bind();
        s_SurfaceBuffers.Program->SetFloat("u_ShowCurvatureMap", showCurvatureMap);
        RenderCommand::DrawIndexed(s_SurfaceBuffers.VAO, indices.size());
    }

    void Renderer::BeginScene(const glm::mat4 &viewProjection)
    {
        Renderer2D::BeginScene(viewProjection);

        s_RenderPass.BeginPass();

        int pickingTextureIndex = 1, clearID = 0;
        s_RenderPass.ClearTexture(pickingTextureIndex, clearID);
    }

    void Renderer::EndScene()
    {
        Renderer2D::EndScene();
        s_RenderPass.EndPass();
        RenderCommand::Clear({RenderBuffer::Color, RenderBuffer::Depth});
        s_RenderPass.Blit();
    }

    void Renderer::Resize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0, 0, width, height);
        s_RenderPass.Resize(width, height);
    }

    void Renderer::ToggleWireframe()
    {
        s_isWireframe = !s_isWireframe;

        switch(s_isWireframe)
        {
            case true:
                RenderCommand::SetWireframeMode();
                break;
            case false:
                RenderCommand::SetFillMode();
                break;
        }
    }

    uint32_t Renderer::ReadPixel(uint32_t textureIndex, int x, int y)
    {
        int height = Core::Application::Get().GetWindow().GetHeight();
        s_RenderPass.GetFrameBuffer()->Bind(FrameBufferBind::Read);
        uint32_t pixel = s_RenderPass.GetFrameBuffer()->ReadPixel(textureIndex, x, height - y);
        s_RenderPass.GetFrameBuffer()->Unbind(FrameBufferBind::Read);
        return pixel;
    }
}
