#include "precompiled.h"

#include "Renderer/Renderer2D.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Texture.h"
#include "Renderer/UniformBuffer.h"

#include "Core/Log.h"

namespace SmartGL
{
    struct QuadVertexLayout
    {
        glm::vec4 Color;
        glm::vec3 Position;
        glm::vec2 TexCoord;
        float TexIndex;
        float TilingFactor;
        uint32_t EntityID;
    };

    struct CircleVertexLayout
    {
        glm::vec4 Color;
        glm::vec3 WorldPosition;
        glm::vec3 Position;
        float Thickness;
        float Fade;
        uint32_t EntityID;
    };

    struct LineVertexLayout
    {
        glm::vec4 Color;
        glm::vec3 Position;
    };

    struct PointVertexLayout
    {
        glm::vec4 Color;
        glm::vec3 Position;
        float Size;
    };

    void Renderer2D::Statistics::Log() const
    {
        SMART_LOG_TRACE("Renderer2D::Statistics:");
        SMART_LOG_TRACE("    DrawCalls: {0}", DrawCalls);
        SMART_LOG_TRACE("    QuadCount: {0}", QuadCount);
        SMART_LOG_TRACE("    LineCount: {0}", LineCount);
        SMART_LOG_TRACE("    TotalVertexCount: {0}", GetTotalVertexCount());
        SMART_LOG_TRACE("    TotalIndexCount: {0}", GetTotalIndexCount());
    };

    struct Renderer2DData
    {

        // circles are rendered as quads
        static const uint32_t MaxQuads = 10000;
        static const uint32_t MaxQuadVertices = MaxQuads * 4;
        static const uint32_t MaxQuadIndices = MaxQuads * 6;

        static const uint32_t MaxLines = 10000;
        static const uint32_t MaxLineVertices = MaxLines * 2;

        static const uint32_t MaxPoints = 1000000;

        static const uint32_t MaxTextureSlots = 16;

        Shared<VertexArray> QuadVertexArray;
        Shared<VertexBuffer> QuadVertexBuffer;
        Shared<Shader> QuadShader;
        Shared<Texture2D> WhiteTexture;

        Shared<VertexArray> CircleVertexArray;
        Shared<VertexBuffer> CircleVertexBuffer;
        Shared<Shader> CircleShader;

        Shared<VertexArray> LineVertexArray;
        Shared<VertexBuffer> LineVertexBuffer;
        Shared<Shader> LineShader;

        Shared<VertexArray> PointVertexArray;
        Shared<VertexBuffer> PointVertexBuffer;
        Shared<Shader> PointShader;

        uint32_t QuadIndexCount = 0;
        QuadVertexLayout *QuadVertexBufferBase = nullptr;
        QuadVertexLayout *QuadVertexBufferPtr = nullptr;

        uint32_t CircleIndexCount = 0;
        CircleVertexLayout *CircleVertexBufferBase = nullptr;
        CircleVertexLayout *CircleVertexBufferPtr = nullptr;

        uint32_t LineVerticesCount = 0;
        LineVertexLayout *LineVertexBufferBase = nullptr;
        LineVertexLayout *LineVertexBufferPtr = nullptr;
        int LineWidth = 1;

        uint32_t PointVerticesCount = 0;
        PointVertexLayout *PointVertexBufferBase = nullptr;
        PointVertexLayout *PointVertexBufferPtr = nullptr;
        static const int PointSize = 1;

        std::array<Shared<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 = white texture

        glm::vec4 QuadVertexPositions[4];

        Renderer2D::Statistics Stats;

        struct CameraData
        {
            glm::mat4 ViewProjection;
        };

        CameraData CameraBuffer;
        Shared<UniformBuffer> CameraUniformBuffer;
    };

    static Renderer2DData s_Data;

    void Renderer2D::Init()
    {
        // quads
        s_Data.QuadVertexArray = CreateShared<VertexArray>();

        s_Data.QuadVertexBuffer = CreateShared<VertexBuffer>(s_Data.MaxQuadVertices * sizeof(QuadVertexLayout));
        s_Data.QuadVertexBuffer->SetLayout({{ShaderDataType::Float4, "a_Color"},
                                            {ShaderDataType::Float3, "a_Position"},
                                            {ShaderDataType::Float2, "a_TexCoord"},
                                            {ShaderDataType::Float, "a_TexIndex"},
                                            {ShaderDataType::Float, "a_TilingFactor"},
                                            {ShaderDataType::Int, "a_EntityID"}});
        s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

        s_Data.QuadVertexBufferBase = new QuadVertexLayout[s_Data.MaxQuadVertices];

        uint32_t *quadIndices = new uint32_t[s_Data.MaxQuadIndices];

        uint32_t offset = 0;
        for (uint32_t i = 0; i < s_Data.MaxQuadIndices; i += 6)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }

        uint32_t count = s_Data.MaxQuadIndices;
        Shared<IndexBuffer> quadIBO = CreateShared<IndexBuffer>(quadIndices, count);
        s_Data.QuadVertexArray->SetIndexBuffer(quadIBO);
        delete[] quadIndices;

        // circles
        s_Data.CircleVertexArray = CreateShared<VertexArray>();

        s_Data.CircleVertexBuffer = CreateShared<VertexBuffer>(s_Data.MaxQuadVertices * sizeof(CircleVertexLayout));
        s_Data.CircleVertexBuffer->SetLayout({{ShaderDataType::Float4, "a_Color"},
                                              {ShaderDataType::Float3, "a_WorldPosition"},
                                              {ShaderDataType::Float3, "a_Position"},
                                              {ShaderDataType::Float, "a_Thickness"},
                                              {ShaderDataType::Float, "a_Fade"},
                                              {ShaderDataType::Int, "a_EntityID"}});

        s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
        s_Data.CircleVertexArray->SetIndexBuffer(quadIBO);
        s_Data.CircleVertexBufferBase = new CircleVertexLayout[s_Data.MaxQuadVertices];

        // lines
        s_Data.LineVertexArray = CreateShared<VertexArray>();

        s_Data.LineVertexBuffer = CreateShared<VertexBuffer>(s_Data.MaxLineVertices * sizeof(LineVertexLayout));
        s_Data.LineVertexBuffer->SetLayout({{ShaderDataType::Float4, "a_Color"},
                                            {ShaderDataType::Float3, "a_Position"}});
        s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
        s_Data.LineVertexBufferBase = new LineVertexLayout[s_Data.MaxLineVertices];

        s_Data.PointVertexArray = CreateShared<VertexArray>();
        s_Data.PointVertexBuffer = CreateShared<VertexBuffer>(s_Data.MaxPoints * sizeof(PointVertexLayout));
        s_Data.PointVertexBuffer->SetLayout({{ShaderDataType::Float4, "a_Color"},
                                            {ShaderDataType::Float3, "a_Position"}});

        s_Data.PointVertexArray->AddVertexBuffer(s_Data.PointVertexBuffer);
        s_Data.PointVertexBufferBase = new PointVertexLayout[s_Data.MaxPoints];

        // shaders

        s_Data.WhiteTexture = CreateShared<Texture2D>(TextureSpecifications());
        uint32_t whiteTextureData = 0xffffffff;
        s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

        s_Data.TextureSlots[0] = s_Data.WhiteTexture;

#if defined(SMART_PLATFORM_LINUX) || defined(SMART_COMPILER_MINGW)
        std::string shaderPath = "SmartGL/assets/shaders/renderer2d/";
#else
        std::string shaderPath = "../SmartGL/assets/shaders/renderer2d/";
#endif

        s_Data.QuadShader = CreateShared<Shader>("Quad", shaderPath + "quad.glsl");
        s_Data.CircleShader = CreateShared<Shader>("Circle", shaderPath + "circle.glsl");
        s_Data.LineShader = CreateShared<Shader>("Line", shaderPath + "line.glsl");
        s_Data.PointShader = CreateShared<Shader>("Point", shaderPath + "point.glsl");

        s_Data.QuadVertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
        s_Data.QuadVertexPositions[1] = {0.5f, -0.5f, 0.0f, 1.0f};
        s_Data.QuadVertexPositions[2] = {0.5f, 0.5f, 0.0f, 1.0f};
        s_Data.QuadVertexPositions[3] = {-0.5f, 0.5f, 0.0f, 1.0f};

        s_Data.CameraUniformBuffer = CreateShared<UniformBuffer>(sizeof(Renderer2DData::CameraData));
        s_Data.CameraUniformBuffer->Bind(0);
    }

    void Renderer2D::Shutdown()
    {
        delete[] s_Data.QuadVertexBufferBase;
        delete[] s_Data.CircleVertexBufferBase;
        delete[] s_Data.LineVertexBufferBase;
        delete[] s_Data.PointVertexBufferBase;

        s_Data.QuadVertexArray.reset();
        s_Data.QuadShader.reset();
        s_Data.QuadVertexBuffer.reset();

        s_Data.CircleVertexArray.reset();
        s_Data.CircleShader.reset();
        s_Data.CircleVertexBuffer.reset();

        s_Data.LineVertexArray.reset();
        s_Data.LineShader.reset();
        s_Data.LineVertexBuffer.reset();

        s_Data.PointVertexArray.reset();
        s_Data.PointShader.reset();
        s_Data.PointVertexBuffer.reset();

        s_Data.WhiteTexture.reset();

        s_Data.CameraUniformBuffer.reset();

        StartBatch();
        ResetStats();
    }

    void Renderer2D::BeginScene(const glm::mat4 &viewProjection)
    {
        s_Data.CameraBuffer.ViewProjection = viewProjection;
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

        StartBatch();
    }

    void Renderer2D::EndScene()
    {
        NextBatch();
    }

    void Renderer2D::StartBatch()
    {
        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.CircleIndexCount = 0;
        s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

        s_Data.LineVerticesCount = 0;
        s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

        s_Data.PointVerticesCount = 0;
        s_Data.PointVertexBufferPtr = s_Data.PointVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::NextBatch()
    {
        Flush();
        StartBatch();
    }

    void Renderer2D::Flush()
    {
        if (s_Data.QuadIndexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t *)s_Data.QuadVertexBufferPtr - (uint8_t *)s_Data.QuadVertexBufferBase);
            s_Data.QuadVertexBuffer->SetData(dataSize, s_Data.QuadVertexBufferBase);

            for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
                s_Data.TextureSlots[i]->Bind(i);

            // Bind textures
            s_Data.QuadShader->Bind();

            RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
            s_Data.Stats.DrawCalls++;
        }

        if (s_Data.CircleIndexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t *)s_Data.CircleVertexBufferPtr - (uint8_t *)s_Data.CircleVertexBufferBase);
            s_Data.CircleVertexBuffer->SetData(dataSize, s_Data.CircleVertexBufferBase);

            s_Data.CircleShader->Bind();
            RenderCommand::DrawIndexed(s_Data.CircleVertexArray, s_Data.CircleIndexCount);
            s_Data.Stats.DrawCalls++;
        }

        if (s_Data.LineVerticesCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t *)s_Data.LineVertexBufferPtr - (uint8_t *)s_Data.LineVertexBufferBase);
            s_Data.LineVertexBuffer->SetData(dataSize, s_Data.LineVertexBufferBase);

            s_Data.LineShader->Bind();
            RenderCommand::SetLineWidth(s_Data.LineWidth);
            RenderCommand::DrawLines(s_Data.LineVertexArray, s_Data.LineVerticesCount);
            s_Data.Stats.DrawCalls++;
        }

        if (s_Data.PointVerticesCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t *)s_Data.PointVertexBufferPtr - (uint8_t *)s_Data.PointVertexBufferBase);
            s_Data.PointVertexBuffer->SetData(dataSize, s_Data.PointVertexBufferBase);

            s_Data.PointShader->Bind();
            RenderCommand::DrawPoints(s_Data.PointVertexArray, s_Data.PointVerticesCount);
            s_Data.Stats.DrawCalls++;
        }
    }

    void Renderer2D::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec4 &color, int entityID)
    {
        DrawQuad({position.x, position.y, 0.0f}, size, color, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::vec2 &size, const glm::vec4 &color, int entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

        DrawQuad(transform, color, entityID);
    }

    void Renderer2D::DrawQuad(const glm::vec3 &position, const glm::mat4 &transform, const glm::vec2 &size, const glm::vec4 &color, int entityID)
    {
        glm::mat4 newTransform = glm::translate(glm::mat4(1.0f), position) * transform * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

        DrawQuad(newTransform, color, entityID);
    }

    void Renderer2D::DrawQuad(const glm::mat4 &transform, const glm::vec4 &color, int entityID)
    {
        constexpr size_t quadVertexCount = 4;
        const float textureIndex = 0.0f; // White Texture
        constexpr glm::vec2 textureCoords[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
        const float tilingFactor = 1.0f;

        if (s_Data.QuadIndexCount >= s_Data.MaxQuadIndices)
            NextBatch();

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr->Color = color;
            s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
            s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
            s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
            s_Data.QuadVertexBufferPtr->EntityID = entityID;
            s_Data.QuadVertexBufferPtr++;
        }

        s_Data.QuadIndexCount += 6;
        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawCircle(const glm::vec2 &position, const glm::vec4 &color, float radius, float thickness, float fade, int entityID)
    {
        DrawCircle({position.x, position.y, 0.0f}, color, radius, thickness, fade, entityID);
    }

    void Renderer2D::DrawCircle(const glm::vec3 &position, const glm::vec4 &color, float radius, float thickness, float fade, int entityID)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), {radius, radius, 1.0f});

        DrawCircle(transform, color, thickness, fade, entityID);
    }

    void Renderer2D::DrawCircle(const glm::vec3 &position, const glm::mat4 &transform, const glm::vec4 &color, float radius, float thickness, float fade, int entityID)
    {
        glm::mat4 newTransform = glm::translate(glm::mat4(1.0f), position) * transform * glm::scale(glm::mat4(1.0f), {radius, radius, 1.0f});

        DrawCircle(newTransform, color, thickness, fade, entityID);
    }

    void Renderer2D::DrawCircle(const glm::mat4 &transform, const glm::vec4 &color, float thickness, float fade, int entityID)
    {
        if (s_Data.CircleIndexCount >= s_Data.MaxQuadIndices)
            NextBatch();

        for (size_t i = 0; i < 4; i++)
        {
            s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
            s_Data.CircleVertexBufferPtr->Position = s_Data.QuadVertexPositions[i] * 2.0f; // scale to [-1, 1]
            s_Data.CircleVertexBufferPtr->Color = color;
            s_Data.CircleVertexBufferPtr->Thickness = thickness;
            s_Data.CircleVertexBufferPtr->Fade = fade;
            s_Data.CircleVertexBufferPtr->EntityID = entityID;
            s_Data.CircleVertexBufferPtr++;
        }

        s_Data.CircleIndexCount += 6;
        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawLine(const glm::vec2 &start, const glm::vec2 &end, const glm::vec4 &color)
    {
        DrawLine({start.x, start.y, 0.0f}, {end.x, end.y, 0.0f}, color);
    }

    void Renderer2D::DrawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec4 &color)
    {
        if (s_Data.LineVerticesCount >= s_Data.MaxLineVertices)
            NextBatch();

        s_Data.LineVertexBufferPtr->Position = start;
        s_Data.LineVertexBufferPtr->Color = color;
        s_Data.LineVertexBufferPtr++;

        s_Data.LineVertexBufferPtr->Position = end;
        s_Data.LineVertexBufferPtr->Color = color;
        s_Data.LineVertexBufferPtr++;

        s_Data.LineVerticesCount += 2;
        s_Data.Stats.LineCount++;
    }

    void Renderer2D::DrawPoint(const glm::vec2 &position, const glm::vec4 &color, float size)
    {
        DrawPoint({position.x, position.y, 0.0f}, color, size);
    }

    void Renderer2D::DrawPoint(const glm::vec3 &position, const glm::vec4 &color, float size)
    {
        if (s_Data.PointVerticesCount >= s_Data.MaxPoints)
            NextBatch();

        s_Data.PointVertexBufferPtr->Position = position;
        s_Data.PointVertexBufferPtr->Color = color;
        s_Data.PointVertexBufferPtr->Size = size;
        s_Data.PointVertexBufferPtr++;

        s_Data.PointVerticesCount++;
        s_Data.Stats.PointCount++;
    }

    void Renderer2D::SetLineWidth(float width)
    {
        s_Data.LineWidth = width;
    }

    void Renderer2D::ResetStats()
    {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }

    Renderer2D::Statistics const &Renderer2D::GetStats()
    {
        return s_Data.Stats;
    }
}
