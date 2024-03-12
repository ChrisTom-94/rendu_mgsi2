#pragma once

#include "Core/Assert.h"

#include "glad/gl.h"

#include "Core/Log.h"

#include <string>
#include <iostream>

namespace SmartGL
{

	enum class ImageFormat
	{
		None = 0,
		R8,
		RGB8,
		RGBA8,
		RGBA16F,
		RGBA32F
	};

	struct TextureSpecifications
	{
		uint32_t Width = 1;
		uint32_t Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		bool GenerateMips = true;
		bool Multisampled = false;
	};

	namespace TextureUtils
	{
		static GLenum ImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:
				return GL_RGB;
			case ImageFormat::RGBA8:
				return GL_RGBA;
			case ImageFormat::RGBA16F:
				return GL_RGBA;
			case ImageFormat::RGBA32F:
				return GL_RGBA;
			default:
				SMART_ASSERT(false, "Unknown ImageFormat!");
			}
			return 0;
		}

		static GLenum ImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:
				return GL_RGB8;
			case ImageFormat::RGBA8:
				return GL_RGBA8;
			case ImageFormat::RGBA16F:
				return GL_RGBA16F;
			case ImageFormat::RGBA32F:
				return GL_RGBA32F;
			default:
				SMART_ASSERT(false, "Unknown ImageFormat!");
			}
			return 0;
		}

		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static const uint8_t GetTextureChannelsSize(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:
				return 3;
			case ImageFormat::RGBA8:
				return 4;
			case ImageFormat::RGBA16F:
				return 4;
			case ImageFormat::RGBA32F:
				return 4;
			default:
				SMART_ASSERT(false, "Unknown ImageFormat!");
			}
			return 0;
		}

		static void CreateTextures(bool multisampled, uint32_t *outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled), count, outID);
		}

		static void BindTexture(bool multisampled, uint32_t id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}

		static void BindTextureUnit(uint32_t id, uint32_t slot)
		{
			glBindTextureUnit(slot, id);
		}

		static void ClearTexture(uint32_t id, GLenum format, GLenum dataType, const void *data)
		{
			glClearTexImage(id, 0, format, dataType, data);
		}

		static const uint32_t GetTexturePixelsCount(uint32_t id)
		{
			int width, height;
			glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_WIDTH, &width);
			glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_HEIGHT, &height);
			return width * height;
		}

		static const uint32_t GetTextureBufferSize(uint32_t id, uint32_t format = GL_RGBA)
		{
			uint32_t size = GetTexturePixelsCount(id) * 16;
			return size;
		}

		static const std::vector<float> GetTextureImage(uint32_t id, uint32_t format = GL_RGBA, uint32_t dataType = GL_FLOAT)
		{
			uint32_t size = GetTextureBufferSize(id);
			std::vector<float> data;
			data.resize(size);
			glGetTextureImage(id, 0, format, dataType, size, data.data());
			return data;
		}
	}

	class Texture
	{

	public:
		virtual ~Texture() = default;

		virtual const TextureSpecifications &GetSpecifications() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual const std::string &GetPath() const = 0;

		virtual void SetData(void *data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool IsLoaded() const = 0;

		virtual bool operator==(const Texture &other) const = 0;
	};

	class Texture2D : public Texture
	{

	public:
		Texture2D(const TextureSpecifications &specification);
		Texture2D(const std::string &path);
		virtual ~Texture2D();

		virtual const TextureSpecifications &GetSpecifications() const override { return m_Specifications; }

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_ProgramID; }

		virtual const std::string &GetPath() const override { return m_Path; }

		virtual void SetData(void *data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool IsLoaded() const override { return m_IsLoaded; }

		virtual bool operator==(const Texture &other) const override
		{
			return m_ProgramID == other.GetRendererID();
		}

	private:
		TextureSpecifications m_Specifications;

		std::string m_Path;
		bool m_IsLoaded = false;
		uint32_t m_Width, m_Height;
		uint32_t m_ProgramID;
		GLenum m_InternalFormat, m_DataFormat;
	};
}
