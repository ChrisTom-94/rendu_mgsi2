#pragma once

#include "Core/TypesDefinitions.h"

#include "Renderer/Shader.h"

#include <string>
#include <unordered_map>

namespace SmartGL
{
    class ShaderLibrary
    {
    public:
        static void Load(const std::string &name, const std::string &filepath);
        static void Load(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc);
        static Shared<Shader> Get(const std::string &name);

    private:
        static std::string FindShaderFile(const std::string &filename);

    private:
        static std::unordered_map<std::string, Shared<Shader>> m_Shaders;
    };
}
