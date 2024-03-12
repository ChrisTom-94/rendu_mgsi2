#pragma once

#include "SmartGL.h"
#include "BSplineSurface.h"

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
    ControlPoint(const glm::vec3 &position, uint8_t id) : Position(position), ID(id) {}
};

namespace TP2_Nurbs
{
    class Renderer
    {
    public:
        static void Init();
        static void ShutDown();

        static void BeginScene(const glm::mat4 &viewProjection);
        static void DrawControlPoints(const std::vector<std::vector<ControlPoint>> &controlPoints);
        static void DrawSurface(std::vector<std::vector<glm::vec3>> &points, std::vector<std::vector<float>> curvatures = {}, bool showCurvatureMap = false);
        static void EndScene();

        static void Resize(uint32_t width, uint32_t height);
        static void ToggleWireframe();

        /**
         * @brief Read a pixel from a texture
         * @brief Used in this application to do a color picking
        */
        static uint32_t ReadPixel(uint32_t textureIndex, int x, int y);
    };
}
