#pragma once

#include "SmartGL.h"
#include "Editor.h"

using namespace SmartGL;

namespace TP_Deformations
{

    enum class DeformationShader
    {
        Twist,
        Pinch
    };

    class Renderer
    {

    public:
        static void Init();
        static void ShutDown();

        static void BeginScene(const Shared<PerspectiveCamera> &camera, const glm::mat4 &transform, Material &material, Light &light);
        static void DrawGeometry(float minT, float maxT);
        static void EndScene();

        static void SetGeometry(const Geometry &geometry);
        static void SetActiveShader(DeformationShader shader);

        static void Resize(uint32_t width, uint32_t height);
    };
};
