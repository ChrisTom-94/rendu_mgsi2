#include "precompiled.h"

#include "Renderer/Texture.h"

#include "stb_image.h"

namespace SmartGL
{
    Texture2D::Texture2D(const TextureSpecifications& specification)
        : m_Specifications(specification), m_Width(specification.Width), m_Height(specification.Height)
    {

        m_DataFormat = TextureUtils::ImageFormatToGLDataFormat(m_Specifications.Format);
        m_InternalFormat = TextureUtils::ImageFormatToGLInternalFormat(m_Specifications.Format);

        TextureUtils::CreateTextures(m_Specifications.Multisampled, &m_ProgramID, 1);
        glTextureStorage2D(m_ProgramID, 1, m_InternalFormat, m_Width, m_Height);

        glTextureParameteri(m_ProgramID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_ProgramID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_ProgramID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_ProgramID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if (m_Specifications.GenerateMips)
            glGenerateTextureMipmap(m_ProgramID);
    }

    Texture2D::Texture2D(const std::string& path)
    {
        m_Path = path;

        int width, height, channels;
		stbi_set_flip_vertically_on_load(1);

        stbi_uc* data = nullptr;
        {
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        }

        SMART_ASSERT(data, "Failed to load image!");

        m_IsLoaded = true;
        m_Width = width;
        m_Height = height;

        GLenum internalFormat = 0, dataFormat = 0;
        if (channels == 4)
        {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (channels == 3)
        {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }

        m_DataFormat = dataFormat;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_ProgramID);
        glTextureStorage2D(m_ProgramID, 1, internalFormat, m_Width, m_Height);

        glTextureParameteri(m_ProgramID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_ProgramID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_ProgramID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_ProgramID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureSubImage2D(m_ProgramID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
    }

    Texture2D::~Texture2D()
    {
        glDeleteTextures(1, &m_ProgramID);
    }

    void Texture2D::SetData(void* data, uint32_t size)
    {
        uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
        SMART_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");

        glTextureSubImage2D(m_ProgramID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
    }

    void Texture2D::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_ProgramID);
    }
}
