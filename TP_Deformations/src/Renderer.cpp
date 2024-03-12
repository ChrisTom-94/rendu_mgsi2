#include "Renderer.h"

namespace TP_Deformations
{
    /// @brief Data needed by the renderer.
    struct RendererData
    {
        // camera uniform buffer
        struct CameraData
        {
            glm::mat4 ViewProjection;
            glm::vec4 CameraPosition; // w = 1.0f (just to align the data to 16 bytes)
        };
        CameraData CameraData;
        Shared<UniformBuffer> CameraUniformBuffer;

        // object uniform buffer
        struct ObjectData
        {
            glm::mat4 Model;
        };
        ObjectData ObjectData;
        Shared<UniformBuffer> ObjectUniformBuffer;

        // material uniform buffer
        struct MaterialData
        {
            glm::vec4 AlbedoAndShininess; // xyz = albedo, w = shininess
        };
        MaterialData MaterialData;
        Shared<UniformBuffer> MaterialUniformBuffer;

        // light uniform buffer
        struct LightData
        {
            glm::vec4 AmbientColor; // w = 1.0f
            glm::vec4 Position;     // w = 1.0f
            glm::vec4 LightColor;   // w = intensity
        };
        LightData LightData;
        Shared<UniformBuffer> LightUniformBuffer;

        /// @brief Vao and Buffers for render passes
        struct Buffers
        {
            Shared<VertexArray> Vao;
            Shared<VertexBuffer> Vbo;
            Shared<IndexBuffer> Ibo;
        };
        Buffers GeometryBuffers; // geometry pass (GBuffer)
        Buffers RenderBuffers;

        Shared<Shader> TwistShader;
        Shared<Shader> PinchShader;
        Shared<Shader> ActiveShader;

        Shared<Shader> BlinnPhongShader;

        const int MaxGeometryVerticesCount = 10000;
        const int MaxGeometryIndicesCount = 10000;
        int GeometryIndicesCount = 0;
    };

    static GBuffer s_GBuffer;
    static RendererData s_Data;

    void Renderer::Init()
    {
        std::string workingDirectory = Core::Application::Get().GetSpecification().WorkingDirectory;
        uint32_t width = Core::Application::Get().GetWindow().GetWidth();
        uint32_t height = Core::Application::Get().GetWindow().GetHeight();

        { // GBuffer setup (store : position, normal, albedo + shininess) using Multiple Render Targets (MRT)
            std::string shaderPath = workingDirectory + "shaders/twist.glsl";
            s_Data.TwistShader = CreateShared<Shader>("Twist", shaderPath);

            shaderPath = workingDirectory + "shaders/pinch.glsl";
            s_Data.PinchShader = CreateShared<Shader>("Pinch", shaderPath);

            s_Data.ActiveShader = s_Data.TwistShader;

            s_GBuffer.Init(width, height);
            s_GBuffer.SetShader(s_Data.ActiveShader);
            s_Data.GeometryBuffers.Vao = CreateShared<VertexArray>();
        }

        { // Uniform Buffers
            s_Data.CameraUniformBuffer = CreateShared<UniformBuffer>(sizeof(RendererData::CameraData));
            s_Data.ObjectUniformBuffer = CreateShared<UniformBuffer>(sizeof(RendererData::ObjectData));
            s_Data.MaterialUniformBuffer = CreateShared<UniformBuffer>(sizeof(RendererData::MaterialData));
            s_Data.LightUniformBuffer = CreateShared<UniformBuffer>(sizeof(RendererData::LightData));

            uint8_t bindingCamera = 0, bindingObject = 1, bindingMaterial = 2, bindingLight = 3;
            s_Data.CameraUniformBuffer->Bind(bindingCamera);
            s_Data.ObjectUniformBuffer->Bind(bindingObject);
            s_Data.MaterialUniformBuffer->Bind(bindingMaterial);
            s_Data.LightUniformBuffer->Bind(bindingLight);
        }

        { // init shading pass
            std::string shaderPath = workingDirectory + "shaders/blinn-phong.glsl";
            s_Data.BlinnPhongShader = CreateShared<Shader>("Blinn-Phong", shaderPath);

            // bind gbuffer textures
            s_Data.BlinnPhongShader->Bind();
            s_Data.BlinnPhongShader->SetInt("u_TexPositions", 0);
            s_Data.BlinnPhongShader->SetInt("u_TexNormals", 1);
            s_Data.BlinnPhongShader->SetInt("u_TexAlbedoSpecular", 2);
            s_Data.BlinnPhongShader->Unbind();
        }

        { // Vao and buffers geometry pass
            s_Data.GeometryBuffers.Vbo = CreateShared<VertexBuffer>(s_Data.MaxGeometryVerticesCount * sizeof(float));
            s_Data.GeometryBuffers.Vbo->SetLayout({{ShaderDataType::Float3, "a_Position"},
                                                   {ShaderDataType::Float3, "a_Normal"},
                                                   {ShaderDataType::Float2, "a_TexCoord"}});
            s_Data.GeometryBuffers.Vao->AddVertexBuffer(s_Data.GeometryBuffers.Vbo);

            s_Data.GeometryBuffers.Ibo = CreateShared<IndexBuffer>(s_Data.MaxGeometryIndicesCount);
            s_Data.GeometryBuffers.Vao->SetIndexBuffer(s_Data.GeometryBuffers.Ibo);
        }

        { // Vao and buffers shading pass
            float quadVertices[] = {
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f};

            uint32_t quadIndices[] = {
                0, 1, 2,
                2, 3, 0};

            s_Data.RenderBuffers.Vao = CreateShared<VertexArray>();

            Shared<VertexBuffer> vbo = CreateShared<VertexBuffer>(sizeof(quadVertices), quadVertices);
            vbo->SetLayout({{ShaderDataType::Float3, "a_Position"},
                            {ShaderDataType::Float2, "a_TexCoord"}});
            s_Data.RenderBuffers.Vao->AddVertexBuffer(vbo);

            Shared<IndexBuffer> ibo = CreateShared<IndexBuffer>(quadIndices, 6);
            s_Data.RenderBuffers.Vao->SetIndexBuffer(ibo);
        }
    }

    void Renderer::BeginScene(const Shared<PerspectiveCamera> &camera, const glm::mat4 &transform, Material &material, Light &light)
    {
        // update uniform buffers

        s_Data.CameraData.ViewProjection = camera->GetViewProjectionMatrix();
        s_Data.CameraData.CameraPosition = camera->GetViewMatrixInverse()[3];
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraData, sizeof(RendererData::CameraData));

        s_Data.ObjectData.Model = transform;
        s_Data.ObjectUniformBuffer->SetData(&s_Data.ObjectData, sizeof(RendererData::ObjectData));

        s_Data.MaterialData.AlbedoAndShininess = glm::vec4(material.Albedo, material.Shininess);
        s_Data.MaterialUniformBuffer->SetData(&s_Data.MaterialData, sizeof(RendererData::MaterialData));

        s_Data.LightData.AmbientColor = glm::vec4(light.AmbientColor, 1.0f);
        s_Data.LightData.Position = glm::vec4(light.Position, 1.0f);
        s_Data.LightData.LightColor = glm::vec4(light.Color, light.Intensity);
        s_Data.LightUniformBuffer->SetData(&s_Data.LightData, sizeof(RendererData::LightData));
    }

    void Renderer::DrawGeometry(float minT, float maxT)
    {
        // render geometry to GBuffer
        s_GBuffer.Bind();
        s_Data.ActiveShader->SetFloat("u_MinT", minT);
        s_Data.ActiveShader->SetFloat("u_MaxT", maxT);
        RenderCommand::Clear({RenderBuffer::Color, RenderBuffer::Depth});
        RenderCommand::DrawIndexed(s_Data.GeometryBuffers.Vao, s_Data.GeometryIndicesCount);
        s_GBuffer.Unbind();
    }

    void Renderer::EndScene()
    {
        // render GBuffer to screen using Blinn-Phong shading
        s_GBuffer.BindTextures();

        s_Data.BlinnPhongShader->Bind();
        RenderCommand::Clear({RenderBuffer::Color, RenderBuffer::Depth});
        RenderCommand::DrawIndexed(s_Data.RenderBuffers.Vao, 6);
    }

    void Renderer::SetGeometry(const Geometry &geometry)
    {
        std::vector<float> vertices = geometry.GetAttributes();
        std::vector<uint32_t> indices = geometry.GetIndices();

        s_Data.GeometryIndicesCount = indices.size();
        s_Data.GeometryBuffers.Vbo->SetData(vertices.size() * sizeof(float), vertices.data());
        s_Data.GeometryBuffers.Ibo->SetData(indices.size(), indices.data());
    }

    void Renderer::SetActiveShader(DeformationShader shader)
    {
        switch (shader)
        {
        case DeformationShader::Twist:
            s_Data.ActiveShader = s_Data.TwistShader;
            break;
        case DeformationShader::Pinch:
            s_Data.ActiveShader = s_Data.PinchShader;
            break;
        default:
            break;
        }
        s_GBuffer.SetShader(s_Data.ActiveShader);
    }

    void Renderer::Resize(uint32_t width, uint32_t height)
    {
        s_GBuffer.Resize(width, height);
    }

    void Renderer::ShutDown()
    {
        s_Data.TwistShader.reset();
        s_Data.PinchShader.reset();
        s_Data.ActiveShader.reset();
        s_Data.BlinnPhongShader.reset();
        s_Data.CameraUniformBuffer.reset();
        s_Data.ObjectUniformBuffer.reset();
        s_Data.MaterialUniformBuffer.reset();
        s_Data.LightUniformBuffer.reset();
        s_Data.RenderBuffers.Vao.reset();
        s_Data.GeometryBuffers.Vao.reset();
    }
}
