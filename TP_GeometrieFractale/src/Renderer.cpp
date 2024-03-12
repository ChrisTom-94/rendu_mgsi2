#include "Renderer.h"

#include "SmartGL.h"

using namespace SmartGL;

namespace TP_GeometrieFractale
{
    struct RendererData
    {
        // used to initialize the mountain vbo and ibo
        // with enough space if user add iterations to the mountain
        const uint32_t MaxVertices = 10000;
        const uint32_t MaxIndices = MaxVertices * 3;

        // mountain and sea buffers
        struct Buffers
        {
            Shared<VertexArray> Vao;
            Shared<VertexBuffer> Vbo;
            Shared<IndexBuffer> Ibo;
            Shared<Shader> Program;
        };
        Buffers MountainBuffers;
        Buffers SeaBuffers;

        // camera, model and light uniform buffers

        struct CameraData
        {
            glm::mat4 ViewProjection;
            glm::vec3 CameraPosition;
        };

        CameraData CameraData;
        Shared<UniformBuffer> CameraUniformBuffer;

        struct ModelData
        {
            glm::mat4 Transform;
        };

        ModelData ModelData;
        Shared<UniformBuffer> ModelUniformBuffer;

        struct LightData
        {
            // use vec4 to align the data to 16 bytes (std140 layout)
            glm::vec4 Ambient = {0.2f, 0.2f, 0.2f, 1.0f};
            glm::vec4 Position = {0.0f, 4.0f, 2.0f, 1.0f};
            glm::vec4 ColorAndIntensity = {1.0f, 0.0f, 0.0f, 1.0f};
        };

        LightData LightData;
        Shared<UniformBuffer> LightUniformBuffer;
    };

    static RendererData s_Data;

    void Renderer::Init()
    {

        std::string workingDirectory = Core::Application::Get().GetSpecification().WorkingDirectory;
        std::string shaderPath = workingDirectory + "shaders/";

        { // init sea buffers
            s_Data.SeaBuffers.Vao = CreateShared<VertexArray>();

            PlaneGeometry plane({10.0f, 10.0f, 1, 1});
            auto vertices = plane.GetAttributes();
            s_Data.SeaBuffers.Vbo = CreateShared<VertexBuffer>(vertices.size() * sizeof(float), vertices.data());

            s_Data.SeaBuffers.Vbo->SetLayout({{ShaderDataType::Float3, "a_Position"},
                                              {ShaderDataType::Float3, "a_Normal"},
                                              {ShaderDataType::Float2, "a_TexCoord"}});
            s_Data.SeaBuffers.Vao->AddVertexBuffer(s_Data.SeaBuffers.Vbo);

            auto indices = plane.GetIndices();
            s_Data.SeaBuffers.Ibo = CreateShared<IndexBuffer>(indices.data(), indices.size());
            s_Data.SeaBuffers.Vao->SetIndexBuffer(s_Data.SeaBuffers.Ibo);

            std::string seaShaderPath = shaderPath + "sea.glsl";
            s_Data.SeaBuffers.Program = CreateShared<Shader>("Sea", seaShaderPath);
        }

        { // init mountain buffers
            s_Data.MountainBuffers.Vao = CreateShared<VertexArray>();

            uint32_t size = s_Data.MaxVertices * sizeof(float) * 2;
            s_Data.MountainBuffers.Vbo = CreateShared<VertexBuffer>(size);
            s_Data.MountainBuffers.Vbo->SetLayout({{ShaderDataType::Float3, "a_Position"},
                                                   {ShaderDataType::Float3, "a_Normal"}});
            s_Data.MountainBuffers.Vao->AddVertexBuffer(s_Data.MountainBuffers.Vbo);

            uint32_t count = s_Data.MaxIndices;
            s_Data.MountainBuffers.Ibo = CreateShared<IndexBuffer>(count);
            s_Data.MountainBuffers.Vao->SetIndexBuffer(s_Data.MountainBuffers.Ibo);

            std::string mountainShaderPath = shaderPath + "mountain.glsl";
            s_Data.MountainBuffers.Program = CreateShared<Shader>("Mountain", mountainShaderPath);
        }

        { // init uniform buffers
            s_Data.CameraUniformBuffer = CreateShared<UniformBuffer>(sizeof(RendererData::CameraData));
            s_Data.CameraUniformBuffer->Bind(0);
            s_Data.ModelUniformBuffer = CreateShared<UniformBuffer>(sizeof(RendererData::ModelData));
            s_Data.ModelUniformBuffer->Bind(1);
            s_Data.LightUniformBuffer = CreateShared<UniformBuffer>(sizeof(RendererData::LightData));
            s_Data.LightUniformBuffer->Bind(2);
        }

        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
    }

    void Renderer::UpdateMountain(const FractalMountain &mountain)
    {
        auto vertices = mountain.GetAttributes();
        s_Data.MountainBuffers.Vbo->SetData(vertices.size() * sizeof(float), vertices.data());

        auto indices = mountain.GetIndices();
        s_Data.MountainBuffers.Ibo->SetData(indices.size(), indices.data());
    }

    void Renderer::DrawMountain(const FractalMountain &mountain, const Maths::Transform &transform, bool isWireframe)
    {
        s_Data.ModelData.Transform = transform.GetLocalMatrix();
        s_Data.ModelUniformBuffer->SetData(&s_Data.ModelData, sizeof(RendererData::ModelData));

        RenderCommand::Clear({RenderBuffer::Color, RenderBuffer::Depth});

        { // draw mountain
            s_Data.MountainBuffers.Program->Bind();
            s_Data.MountainBuffers.Program->SetInt("u_IsWireframe", isWireframe);
            s_Data.MountainBuffers.Program->SetFloat("u_MaxHeight", mountain.GetMaxHeight());
            auto count = mountain.GetIndices().size();
            RenderCommand::DrawIndexed(s_Data.MountainBuffers.Vao, count);
        }

        // do not draw sea if wireframe mode is enabled
        // so we can see the mountain wireframe better
        if(isWireframe)
            return;

        { // draw sea
            s_Data.SeaBuffers.Program->Bind();
            RenderCommand::DrawIndexed(s_Data.SeaBuffers.Vao, 6);
        }
    }

    void Renderer::BeginScene(const Shared<PerspectiveCamera> &camera, const Light &light)
    {
        // update camera and light uniform buffers
        s_Data.CameraData.ViewProjection = camera->GetViewProjectionMatrix();
        s_Data.CameraData.CameraPosition = glm::vec3(camera->GetViewMatrixInverse()[3]);
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraData, sizeof(RendererData::CameraData));

        s_Data.LightData.Position = glm::vec4(light.Position, 1.0f);
        s_Data.LightData.ColorAndIntensity = glm::vec4(light.Color, light.Intensity);
        s_Data.LightUniformBuffer->SetData(&s_Data.LightData, sizeof(RendererData::LightData));
    }

    void Renderer::Shutdown()
    {
        s_Data.MountainBuffers.Program.reset();
        s_Data.SeaBuffers.Program.reset();
        s_Data.MountainBuffers.Vao.reset();
        s_Data.MountainBuffers.Vbo.reset();
        s_Data.MountainBuffers.Ibo.reset();
        s_Data.SeaBuffers.Vao.reset();
        s_Data.SeaBuffers.Vbo.reset();
        s_Data.SeaBuffers.Ibo.reset();
        s_Data.CameraUniformBuffer.reset();
        s_Data.ModelUniformBuffer.reset();
        s_Data.LightUniformBuffer.reset();
    }
}
