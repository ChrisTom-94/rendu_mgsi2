#include "precompiled.h"

#include "Renderer/GraphicsContext.h"

#include "Core/Assert.h"

#include "GLFW/glfw3.h"
#include "glad/gl.h"

namespace SmartGL
{

    Unique<GraphicsContext::Specifications> GraphicsContext::s_GraphicsContextSpecifications = CreateUnique<GraphicsContext::Specifications>();

    GraphicsContext::GraphicsContext(GLFWwindow *windowHandle)
        : m_WindowHandle(windowHandle)
    {
        SMART_ASSERT(m_WindowHandle, "Window handle is null!");
    }

    void GraphicsContext::Init()
    {
        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGL((GLADloadfunc)glfwGetProcAddress);

        SMART_ASSERT(status, "Failed to initialize Glad!");

        SMART_ASSERT(GL_MAJOR_VERSION >= SMART_MIN_GL_VERSION_MAJOR, "OpenGL Version < 3.3");
        SMART_ASSERT(GL_MINOR_VERSION >= SMART_MIN_GL_VERSION_MINOR, "OpenGL Version < 3.3");

        s_GraphicsContextSpecifications->Renderer = (char *)glGetString(GL_RENDERER);
        s_GraphicsContextSpecifications->Vendor = (char *)glGetString(GL_VENDOR);
        s_GraphicsContextSpecifications->GlVersion = (char *)glGetString(GL_VERSION);
        s_GraphicsContextSpecifications->GlslVersion = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &s_GraphicsContextSpecifications->MaxTextureSlots);
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &s_GraphicsContextSpecifications->MaxVertexAttributes);

        // split gl version string to get major and minor version
        std::string glVersion = s_GraphicsContextSpecifications->GlVersion;
        std::string delimiter = ".";
        size_t pos = 0;
        std::string token;
        while ((pos = glVersion.find(delimiter)) != std::string::npos)
        {
            token = glVersion.substr(0, pos);
            glVersion.erase(0, pos + delimiter.length());
        }
        s_GraphicsContextSpecifications->GlMajor = std::stoi(token);
        s_GraphicsContextSpecifications->GlMinor = std::stoi(glVersion);

        s_GraphicsContextSpecifications->Log();
    }

    void GraphicsContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
}
