#include "precompiled.h"

#include "Renderer/ShaderLibrary.h"

#include "Core/Application.h"

namespace SmartGL
{
    std::unordered_map<std::string, Shared<Shader>> ShaderLibrary::m_Shaders;

    void ShaderLibrary::Load(const std::string &name, const std::string &filepath)
    {
        Shared<Shader> shader = CreateShared<Shader>(name, filepath);
        m_Shaders[name] = shader;

    }

    void ShaderLibrary::Load(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc)
    {
        Shared<Shader> shader = CreateShared<Shader>(name, vertexSrc, fragmentSrc);
        m_Shaders[name] = shader;
    }

    Shared<Shader> ShaderLibrary::Get(const std::string &name)
    {
        SMART_ASSERT(m_Shaders.find(name) != m_Shaders.end(), "Shader not found!");
        return m_Shaders[name];
    }

    std::string ShaderLibrary::FindShaderFile(const std::string &filename)
    {
        std::string currentDir = Core::Application::Get().GetSpecification().WorkingDirectory;

        #if defined(SMART_PLATFORM_LINUX) || defined(SMART_COMPILER_MINGW)
        currentDir += "/";
#else
        currentDir += "../";
#endif

        std::string filepath = currentDir + "assets/shaders/" + filename;

        if(std::filesystem::exists(filepath))
            return filepath;

        std::string filePathSmartGL = currentDir + "../SmartGL/assets/shaders/" + filename;

        SMART_ASSERT(std::filesystem::exists(filePathSmartGL), "Shader file not found!");

        return filePathSmartGL;
    }
}
