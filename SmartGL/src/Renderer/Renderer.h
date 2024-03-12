#pragma once

#include "Core/TypesDefinitions.h"

#include "Renderer/Camera.h"

namespace SmartGL
{
    class Renderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void OnWindowResize(uint32_t width, uint32_t height);

        static void BeginScene(Camera &camera);
        static void EndScene();

    private:
        struct RendererData
        {
            glm::mat4 ViewProjectionMatrix;
        };
        static Unique<RendererData> s_RendererData;
    };
}
