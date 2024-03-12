#pragma once

#include "FractalMountain.h"

#include "SmartGL.h"

using namespace SmartGL;

namespace TP_GeometrieFractale
{
    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void UpdateMountain(const FractalMountain &mountain);
        static void DrawMountain(const FractalMountain &mountain, const Maths::Transform &transform, bool isWireframe = false);
        static void BeginScene(const Shared<PerspectiveCamera> &camera, const Light &light);
    };
}
