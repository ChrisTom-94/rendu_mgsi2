#pragma once

#include "Core/Log.h"

#include <iostream>

struct GLFWwindow;

#define SMART_MIN_GL_VERSION_MAJOR 3
#define SMART_MIN_GL_VERSION_MINOR 3

namespace SmartGL
{

    class GraphicsContext
    {

    public:
        struct Specifications
        {
            std::string Renderer = "Unknown";
            std::string Vendor = "Unknown";
            std::string GlMinor = "0";
            std::string GlMajor = "0";
            std::string GlVersion = "0";
            std::string GlslVersion = "0";
            int MaxTextureSlots = 0;
            int MaxVertexAttributes = 0;

            inline void Log()
            {
               SMART_LOG_INFO("OpenGL Specifications:");
               SMART_LOG_INFO("  Renderer: {0}", Renderer);
               SMART_LOG_INFO("  Vendor: {0}", Vendor);
               SMART_LOG_INFO("  OpenGL Version: {0}", GlVersion);
               SMART_LOG_INFO("  GLSL Version: {0}", GlslVersion);
               SMART_LOG_INFO("  Max Texture Units: {0}", MaxTextureSlots);
               SMART_LOG_INFO("  Max Vertex Attributes: {0}", MaxVertexAttributes);
            }

            inline float GetGlVersion() const { return std::stof(GlVersion); }
        };

    public:
        GraphicsContext(GLFWwindow *windowHandle);

        void Init();
        void SwapBuffers();

        static inline Specifications &GetSpecifications() { return *s_GraphicsContextSpecifications; }

    private:
        GLFWwindow *m_WindowHandle;

    private:
        static Unique<Specifications> s_GraphicsContextSpecifications;
    };
}
