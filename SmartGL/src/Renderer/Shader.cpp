#include "precompiled.h"

#include "Renderer/Shader.h"

#include "Core/Core.h"
#include "Core/Log.h"

namespace SmartGL
{

    static GLenum ShaderTypeFromString(const std::string &type)
    {
        if (type == "vertex")
            return GL_VERTEX_SHADER;
        if (type == "fragment" || type == "pixel")
            return GL_FRAGMENT_SHADER;

        SMART_ASSERT(false, "Unknown shader type!");
        return 0;
    }

    Shader::Shader(const std::string &name, const std::string &filepath) : m_Name(name)
    {
        ShaderSources sources = Parse(filepath);
        m_ProgramID = Link(sources.VertexSource, sources.FragmentSource);
    }

    Shader::Shader(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc) : m_Name(name)
    {
        m_ProgramID = Link(vertexSrc, fragmentSrc);
    }

    Shader::~Shader()
    {
        glDeleteProgram(m_ProgramID);
    }

    ShaderSources Shader::Parse(const std::string &filepath)
    {

        std::string fileFormat = filepath.substr(filepath.find_last_of(".") + 1);
        SMART_ASSERT(fileFormat == "vert" || fileFormat == "frag" || fileFormat == "glsl", "Shader file format is not supported!");

        if (fileFormat == "glsl")
            return ParseGLSL(filepath);

        std::string cwd = std::filesystem::current_path().string();
        std::string filepathWithCwd = cwd + "/" + filepath;

        std::stringstream ss[2];
        for (int i = 0; i < 2; i++)
        {
            std::ifstream stream(filepathWithCwd + (i == 0 ? ".vert" : ".frag"));

            SMART_ASSERT(stream.is_open(), "Failed to open shader file!");

            std::string line;
            while (getline(stream, line))
            {
                ss[i] << line << "\n";
            }

            stream.close();
        }

        return {ss[0].str(), ss[1].str()};
    }

    std::string Shader::ReadFile(const std::string &filepath)
    {
        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
        if (in)
        {
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            if (size != -1)
            {
                result.resize(size);
                in.seekg(0, std::ios::beg);
                in.read(&result[0], size);
            }
            else
                SMART_LOG_ERROR("Could not read from file '{0}'", filepath);
        }
        else
            SMART_LOG_ERROR("Could not open file '{0}'", filepath);

        return result;
    }

    ShaderSources Shader::ParseGLSL(const std::string &filepath)
    {
        std::string cwd = std::filesystem::current_path().string();
        std::string filepathWithCwd = cwd + "/" + filepath;

        ShaderSources shaderSources;
        std::string source = ReadFile(filepathWithCwd);

        const char *typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0);
        while (pos != std::string::npos)
        {
            size_t eol = source.find_first_of("\r\n", pos);
            SMART_ASSERT(eol != std::string::npos, "Syntax error");
            size_t begin = pos + typeTokenLength + 1;
            std::string type = source.substr(begin, eol - begin);

            SMART_ASSERT(type == "vertex" || type == "fragment" || type == "pixel", "Invalid shader type specified");

            size_t nextLinePos = source.find_first_not_of("\r\n", eol);
            SMART_ASSERT(nextLinePos != std::string::npos, "Syntax error");
            pos = source.find(typeToken, nextLinePos);

            if (type == "vertex")
                shaderSources.VertexSource = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
            else if (type == "fragment" || type == "pixel")
                shaderSources.FragmentSource = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
        }

        return shaderSources;
    }

    uint32_t Shader::Compile(ShaderType type, const std::string &source)
    {
        unsigned int shaderId = glCreateShader((GLenum)type);
        const char *src = source.c_str();
        glShaderSource(shaderId, 1, &src, nullptr);
        glCompileShader(shaderId);

        int result;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);

        if (result == GL_TRUE)
            return shaderId;

        int length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *)MALLOC(length, sizeof(type));

        glGetShaderInfoLog(shaderId, length, &length, message);
        std::string shaderType = (GLenum)type == GL_VERTEX_SHADER ? "vertex" : "fragment";
        SMART_LOG_ERROR("Failed to compile {0} shader {1} ! \n {2}", shaderType, m_Name, message);
        glDeleteShader(shaderId);
        return 0;
    }

    uint32_t Shader::Link(const std::string &vertexSrc, const std::string &fragmentSrc)
    {
        uint32_t program = glCreateProgram();
        uint32_t vs = Compile(ShaderType::VERTEX, vertexSrc);
        uint32_t fs = Compile(ShaderType::FRAGMENT, fragmentSrc);

        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        glValidateProgram(program);
        glDeleteShader(vs);
        glDeleteShader(fs);

        int result;
        glGetProgramiv(program, GL_LINK_STATUS, &result);

        if (result == GL_FALSE)
        {
            int length;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            char *message = (char *)MALLOC(length, sizeof(char));
            glGetProgramInfoLog(program, length, &length, message);
            SMART_LOG_ERROR("Failed to link shader program {0} !", m_Name);
            glDeleteProgram(program);

            int vsLength;
            glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &vsLength);
            char *vsMessage = (char *)MALLOC(vsLength, sizeof(char));
            glGetShaderInfoLog(vs, vsLength, &vsLength, vsMessage);
            if (vsLength > 0)
                SMART_LOG_ERROR("Vertex shader error: {0}", std::string(vsMessage));

            int fsLength;
            glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &fsLength);
            char *fsMessage = (char *)MALLOC(fsLength, sizeof(char));
            glGetShaderInfoLog(fs, fsLength, &fsLength, fsMessage);
            if (fsLength > 0)
                SMART_LOG_ERROR("Fragment shader error: {0}", std::string(fsMessage));

            glDeleteShader(vs);
            glDeleteShader(fs);

            SMART_BREAK();
        }

        return program;
    }

    void Shader::Bind() const
    {
        glUseProgram(m_ProgramID);
    }

    void Shader::Unbind() const
    {
        glUseProgram(0);
    }

    void Shader::SetInt(const std::string &name, int value)
    {
        UploadUniformInt(name, value);
    }

    void Shader::SetIntArray(const std::string &name, int *values, uint32_t count)
    {
        UploadUniformIntArray(name, values, count);
    }

    void Shader::SetFloat(const std::string &name, float value)
    {
        UploadUniformFloat(name, value);
    }

    void Shader::SetFloat2(const std::string &name, const glm::vec2 &value)
    {
        UploadUniformFloat2(name, value);
    }

    void Shader::SetFloat3(const std::string &name, const glm::vec3 &value)
    {
        UploadUniformFloat3(name, value);
    }

    void Shader::SetFloat4(const std::string &name, const glm::vec4 &value)
    {
        UploadUniformFloat4(name, value);
    }

    void Shader::SetMat3(const std::string &name, const glm::mat3 &value)
    {
        UploadUniformMat3(name, value);
    }

    void Shader::SetMat4(const std::string &name, const glm::mat4 &value)
    {
        UploadUniformMat4(name, value);
    }

    void Shader::UploadUniformInt(const std::string &name, int value)
    {
        GLint location = GetUniformLocation(name.c_str());
        glUniform1i(location, value);
    }

    void Shader::UploadUniformIntArray(const std::string &name, int *values, uint32_t count)
    {
        GLint location = GetUniformLocation(name.c_str());
        glUniform1iv(location, count, values);
    }

    void Shader::UploadUniformFloat(const std::string &name, float value)
    {
        GLint location = GetUniformLocation(name.c_str());
        glUniform1f(location, value);
    }

    void Shader::UploadUniformFloat2(const std::string &name, const glm::vec2 &value)
    {
        GLint location = GetUniformLocation(name.c_str());
        glUniform2f(location, value.x, value.y);
    }

    void Shader::UploadUniformFloat3(const std::string &name, const glm::vec3 &value)
    {
        GLint location = GetUniformLocation(name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void Shader::UploadUniformFloat4(const std::string &name, const glm::vec4 &value)
    {
        GLint location = GetUniformLocation(name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void Shader::UploadUniformMat3(const std::string &name, const glm::mat3 &value)
    {
        GLint location = GetUniformLocation(name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::UploadUniformMat4(const std::string &name, const glm::mat4 &value)
    {
        GLint location = GetUniformLocation(name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    GLint Shader::GetUniformLocation(const std::string &name) const
    {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
            return m_UniformLocationCache[name];

        GLint location = glGetUniformLocation(m_ProgramID, name.c_str());
        if (location == -1)
            SMART_LOG_WARN("Uniform '{0}' doesn't exist!", name);

        m_UniformLocationCache[name] = location;
        return location;
    }
}
