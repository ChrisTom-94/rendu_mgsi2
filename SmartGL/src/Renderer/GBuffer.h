#pragma once

#include "Renderer/FrameBuffer.h"
#include "Renderer/Texture.h"
#include "Renderer/Shader.h"

#include "Core/TypesDefinitions.h"

namespace SmartGL
{

    struct GBufferSpecifications
    {
        uint32_t Width;
        uint32_t Height;
        std::string ShaderPath;
    };

    class GBuffer
    {

    public:
        GBuffer();
        GBuffer(uint32_t width, uint32_t height);
        ~GBuffer();

        void Init(uint32_t width, uint32_t height);
        void Bind();
        void BindTextures();
        void Unbind();
        void Clear();
        void Resize(uint32_t width, uint32_t height);

        void SetShader(const Shared<Shader> &shader) {
            if(m_Shader != nullptr) m_Shader.reset();
            m_Shader = shader;
        }

    private:
        Shared<FrameBuffer> m_FrameBuffer;
        Shared<Shader> m_Shader;
    };

}
