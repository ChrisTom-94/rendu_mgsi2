#include "Renderer.h"

namespace TP1_Nurbs
{
    static RenderPass s_RenderPass;

    void Renderer::Init()
    {
        float width = Core::Application::Get().GetWindow().GetWidth();
        float height = Core::Application::Get().GetWindow().GetHeight();

        RenderCommand::SetViewport(0, 0, width, height);

        RenderPassSpecifications renderPassSpecs;
        renderPassSpecs.FrameBufferSpecs.Width = width;
        renderPassSpecs.FrameBufferSpecs.Height = height;
        renderPassSpecs.FrameBufferSpecs.Attachments = {
            // texture format,                   texture data type
            {FrameBufferAttachmentFormat::RGBA8, FrameBufferTextureDataType::UnsignedByte},       // color
            {FrameBufferAttachmentFormat::RED_INTEGER, FrameBufferTextureDataType::UnsignedByte}, // picking
            {FrameBufferAttachmentFormat::Depth}};
        renderPassSpecs.ClearColor = {0.1f, 0.1f, 0.1f, 1.0f};

        s_RenderPass.SetSpecifications(renderPassSpecs);
    }

    void Renderer::BeginScene(const glm::mat4 &viewProjection)
    {
        // setup global uniforms
        Renderer2D::BeginScene(viewProjection);

        // clear picking texture before to store control points ids
        int pickingTextureIndex = 1, clearID = 0;
        s_RenderPass.GetFrameBuffer()->ClearTextureAttachment(pickingTextureIndex, &clearID);

        // bind render pass framebuffer
        s_RenderPass.BeginPass();
    }

    void Renderer::EndScene()
    {
        // flush all draw calls
        Renderer2D::EndScene();

        // unbind render pass framebuffer
        s_RenderPass.EndPass();

        RenderCommand::Clear({RenderBuffer::Color, RenderBuffer::Depth});
        s_RenderPass.Blit(); // blit to screen fbo
    }

#pragma region Draw Functions
    void Renderer::DrawBSpline(const std::vector<glm::vec3> &points)
    {
        glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
        float radius = 0.01f;
        float thickness = 1.0f;
        float fade = 0.005f;

        for (uint32_t i = 0; i < points.size(); i++)
            Renderer2D::DrawCircle(points[i], color, radius, thickness, fade);
    }

    void Renderer::DrawControlPoints(const std::vector<ControlPoint> &controlPoints)
    {
        glm::vec4 color;
        float radius = 0.1f;
        float thickness = 1.0f;
        float fade = 0.005f;

        for (uint32_t i = 0; i < controlPoints.size(); i++)
        {
            const ControlPoint &point = controlPoints[i];
            color = (point.Selected || point.Hovered) ? point.SelectedColor : point.Color;
            Renderer2D::DrawCircle(point.Position, color, radius, thickness, fade, point.ID);

            if (i < controlPoints.size() - 1)
                Renderer2D::DrawLine(point.Position, controlPoints[i + 1].Position, {0.0f, 1.0f, 0.0f, 0.5f});
        }
    }

    void Renderer::DrawFrenetFrame(const glm::vec3 curvePoint, const CurveFrenetFrameComponents &frenetFrame)
    {
        auto [tangent, normal, binormal] = frenetFrame;

        glm::vec4 color = {1.0f, 0.0f, 0.0f, 1.0f};
        Renderer2D::DrawLine(curvePoint, curvePoint + tangent, color);

        color = {0.0f, 1.0f, 0.0f, 1.0f};
        Renderer2D::DrawLine(curvePoint, curvePoint + normal, color);

        color = {0.0f, 0.0f, 1.0f, 1.0f};
        Renderer2D::DrawLine(curvePoint, curvePoint + binormal, color);
    }

    void Renderer::DrawCurvature(const glm::vec3 curvePoint, const CurveFrenetFrameComponents &frenetFrame, float curvature)
    {
        float radius = 1.0f / curvature;
        glm::vec3 center = curvePoint + (radius * frenetFrame.Normal);

        glm::vec4 color = {0.2f, 1.0f, 1.0f, 1.0f};

        float angle = 0.0f;
        int nbPoints = 32;
        float step = 2 * M_PI / nbPoints;

        for (int i = 0; i < nbPoints; i++)
        {
            glm::vec3 p1 = center + frenetFrame.Normal * radius * glm::cos(angle) + radius * frenetFrame.Tangent * glm::sin(angle);
            glm::vec3 p2 = center + frenetFrame.Normal * radius * glm::cos(angle + step) + radius * frenetFrame.Tangent * glm::sin(angle + step);
            Renderer2D::DrawLine(p1, p2, color);
            angle += step;
        }
    }

    void Renderer::DrawSurfaceAlongCurve(const BSplineCurve &spline, float fromT, float toT)
    {
        if (fromT == -1.0f)
            fromT = spline.GetMinT();

        if (toT == -1.0f)
            toT = spline.GetMaxT();

        auto points = spline.GetPoints();

        glm::vec4 color = {1.0f, 0.0f, 0.0f, 0.5f};
        float radius = 0.5f;

        float t = fromT;
        float deltaT = 0.01f;

        int nbPointsPerCircle = 32;
        float angleStep = 2 * M_PI / nbPointsPerCircle;

        while (t < toT)
        {
            auto point = spline.EvaluateAt(t);
            auto [tangent, normal, binormal] = spline.GetFrenetFrameAt(t);

            float angle = 0.0f;

            for (int i = 0; i < nbPointsPerCircle; i++)
            {
                glm::vec3 p1 = point + radius * normal * glm::cos(angle) + radius * binormal * glm::sin(angle);
                glm::vec3 p2 = point + radius * normal * glm::cos(angle + angleStep) + radius * binormal * glm::sin(angle + angleStep);
                Renderer2D::DrawLine(p1, p2, color);

                angle += angleStep;
            }
            t += deltaT;
        }
    }
#pragma endregion

    void Renderer::Resize(uint32_t width, uint32_t height)
    {
        RenderCommand::SetViewport(0, 0, width, height);
        s_RenderPass.Resize(width, height);
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
