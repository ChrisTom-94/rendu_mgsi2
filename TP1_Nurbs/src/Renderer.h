#pragma once

#include "SmartGL.h"
#include "BSplineCurve.h"

using namespace SmartGL;

struct ControlPoint
{
    glm::vec4 Color = {0.0f, 1.0f, 0.0f, 1.0f};
    glm::vec4 SelectedColor = {1.0f, 1.0f, 0.0f, 1.0f};
    glm::vec3 Position;
    uint8_t ID;
    bool Selected = false;
    bool Hovered = false;

    ControlPoint() = default;
    ControlPoint(const glm::vec3 &position, uint8_t id)
        : Position(position), ID(id) {}
};

namespace TP1_Nurbs
{
    class Renderer
    {
    public:
        static void Init();

        static void BeginScene(const glm::mat4 &viewProjection);
        static void EndScene();

        static void DrawBSpline(const std::vector<glm::vec3> &points);
        static void DrawControlPoints(const std::vector<ControlPoint> &controlPoints);
        static void DrawFrenetFrame(const glm::vec3 curvePoint, const CurveFrenetFrameComponents &frenetFrame);
        static void DrawCurvature(const glm::vec3 curvePoint, const CurveFrenetFrameComponents &frenetFrame, float curvature);
        static void DrawSurfaceAlongCurve(const BSplineCurve &spline, float fromT = -1.0f, float toT = -1.0f);

        static void Resize(uint32_t width, uint32_t height);

        /**
         * @brief Read a pixel from a texture
         * @brief Used in this application to do a color picking
        */
        static uint32_t ReadPixel(uint32_t textureIndex, int x, int y);
    };
}
