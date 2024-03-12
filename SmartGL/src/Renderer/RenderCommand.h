#pragma once

#include "Renderer/VertexArray.h"

#include <initializer_list>

namespace SmartGL
{

    enum class RenderBuffer : uint32_t
    {
        Color = GL_COLOR_BUFFER_BIT,
        Depth = GL_DEPTH_BUFFER_BIT,
        Stencil = GL_STENCIL_BUFFER_BIT
    };

    class RenderCommand
    {
    public:
        static void Init();

        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

        static void SetClearColor(const glm::vec4 &color);

        static void Clear(RenderBuffer buffer);
        static void Clear(std::initializer_list<RenderBuffer> buffers);

        static void DrawIndexed(const Shared<VertexArray> &vertexArray, uint32_t indexCount = 0);
        static void DrawQuadIndexed(const Shared<VertexArray> &vertexArray, uint32_t indexCount = 0);
        static void DrawTriangles(const Shared<VertexArray> &vertexArray, uint32_t vertexCount, uint32_t first = 0);
        static void DrawPoints(const Shared<VertexArray> &vertexArray, uint32_t vertexCount, uint32_t first = 0);
        static void DrawLines(const Shared<VertexArray> &vertexArray, uint32_t vertexCount, uint32_t first = 0);
        static void DrawQuads(const Shared<VertexArray> &vertexArray, uint32_t vertexCount, uint32_t first = 0);

        static void SetLineWidth(float width);
        static void SetPointSize(float size);

        static void EnableDepthTest();
        static void EnableBlend();
        static void EnableFaceCulling();
        static void DisableDepthTest();
        static void DisableBlend();
        static void DisableFaceCulling();
        static void SetWireframeMode();
        static void SetFillMode();
    };
}
