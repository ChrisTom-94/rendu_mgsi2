#include "precompiled.h"

#include "Renderer/RenderCommand.h"
#include "Renderer/GraphicsContext.h"

namespace SmartGL
{
    void OpenGLMessageCallback(
        unsigned source,
        unsigned type,
        unsigned id,
        unsigned severity,
        int length,
        const char *message,
        const void *userParam)
    {

        auto const src_str = [source]()
        {
            switch (source)
            {
            case GL_DEBUG_SOURCE_API:
                return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                return "WINDOW SYSTEM";
            case GL_DEBUG_SOURCE_SHADER_COMPILER:
                return "SHADER COMPILER";
            case GL_DEBUG_SOURCE_THIRD_PARTY:
                return "THIRD PARTY";
            case GL_DEBUG_SOURCE_APPLICATION:
                return "APPLICATION";
            case GL_DEBUG_SOURCE_OTHER:
                return "OTHER";
            default:
                return "UNKNOWN";
            }
        }();

        auto const type_str = [type]()
        {
            switch (type)
            {
            case GL_DEBUG_TYPE_ERROR:
                return "ERROR";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                return "DEPRECATED_BEHAVIOR";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                return "UNDEFINED_BEHAVIOR";
            case GL_DEBUG_TYPE_PORTABILITY:
                return "PORTABILITY";
            case GL_DEBUG_TYPE_PERFORMANCE:
                return "PERFORMANCE";
            case GL_DEBUG_TYPE_MARKER:
                return "MARKER";
            case GL_DEBUG_TYPE_OTHER:
                return "OTHER";
            default:
                return "UNKNOWN";
            }
        }();

        auto const severity_str = [severity]()
        {
            switch (severity)
            {
            case GL_DEBUG_SEVERITY_HIGH:
                return "HIGH";
            case GL_DEBUG_SEVERITY_MEDIUM:
                return "MEDIUM";
            case GL_DEBUG_SEVERITY_LOW:
                return "LOW";
            case GL_DEBUG_SEVERITY_NOTIFICATION:
                return "NOTIFICATION";
            default:
                return "UNKNOWN";
            }
        }();

        std::string result;
        result += "[Severity: " + std::string(severity_str);
        result += " - Source: " + std::string(src_str);
        result += " - Type: " + std::string(type_str);
        result += "] Message: " + std::string(message);

        SMART_LOG_ERROR("[OpenGL Error : {0}", result);
    }

    void RenderCommand::Init()
    {

        GraphicsContext::Specifications &specifications = GraphicsContext::GetSpecifications();

        float glMinVersionForDebug = 4.3f;
        if (specifications.GetGlVersion() >= glMinVersionForDebug)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(OpenGLMessageCallback, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_PROGRAM_POINT_SIZE);
    }

    void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
    }

    void RenderCommand::SetClearColor(const glm::vec4 &color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void RenderCommand::Clear(RenderBuffer buffer)
    {
        glClear((uint32_t)buffer);
    }

    void RenderCommand::Clear(std::initializer_list<RenderBuffer> buffers)
    {
        uint32_t mask = 0;
        for (auto buffer : buffers)
            mask |= (uint32_t)buffer;
        glClear(mask);
    }

    void RenderCommand::DrawIndexed(const Shared<VertexArray> &vertexArray, uint32_t indexCount)
    {
        vertexArray->Bind();
        uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }

    void RenderCommand::DrawQuadIndexed(const Shared<VertexArray> &vertexArray, uint32_t indexCount)
    {
        vertexArray->Bind();
        uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
        glDrawElements(GL_QUADS, count, GL_UNSIGNED_INT, nullptr);
    }

    void RenderCommand::DrawTriangles(const Shared<VertexArray> &vertexArray, uint32_t vertexCount, uint32_t first)
    {
        vertexArray->Bind();
        glDrawArrays(GL_TRIANGLES, first, vertexCount);
    }

    void RenderCommand::DrawPoints(const Shared<VertexArray> &vertexArray, uint32_t vertexCount, uint32_t first)
    {
        vertexArray->Bind();
        glDrawArrays(GL_POINTS, first, vertexCount);
    }

    void RenderCommand::SetPointSize(float size)
    {
        glPointSize(size);
    }

    void RenderCommand::DrawLines(const Shared<VertexArray> &vertexArray, uint32_t vertexCount, uint32_t first)
    {
        vertexArray->Bind();
        glDrawArrays(GL_LINES, first, vertexCount);
    }

    void RenderCommand::SetLineWidth(float width)
    {
        glLineWidth(width);
    }

    void RenderCommand::DrawQuads(const Shared<VertexArray> &vertexArray, uint32_t vertexCount, uint32_t first)
    {
        vertexArray->Bind();
        glDrawArrays(GL_QUADS, first, vertexCount);
    }

    void RenderCommand::EnableDepthTest()
    {
        glEnable(GL_DEPTH_TEST);
    }

    void RenderCommand::EnableBlend()
    {
        glEnable(GL_BLEND);
    }

    void RenderCommand::EnableFaceCulling()
    {
        glEnable(GL_CULL_FACE);
    }

    void RenderCommand::DisableDepthTest()
    {
        glDisable(GL_DEPTH_TEST);
    }

    void RenderCommand::DisableBlend()
    {
        glDisable(GL_BLEND);
    }

    void RenderCommand::DisableFaceCulling()
    {
        glDisable(GL_CULL_FACE);
    }

    void RenderCommand::SetWireframeMode()
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void RenderCommand::SetFillMode()
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
