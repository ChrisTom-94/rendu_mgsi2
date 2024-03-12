#pragma once

#include "Core/TypesDefinitions.h"

#include "Renderer/FrameBuffer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"

namespace SmartGL
{

	static const uint32_t DefaultRendererTarget = 0;

	struct RenderPassSpecifications
	{
		FrameBufferSpecifications FrameBufferSpecs;
		glm::vec4 ClearColor;
		Shared<Shader> Program;
		uint32_t RenderTarget = DefaultRendererTarget;
	};

	class RenderPass
	{
	public:
		RenderPass() = default;
		RenderPass(RenderPassSpecifications specs);
		~RenderPass();

		void SetSpecifications(const RenderPassSpecifications &spec);
		void BindShader();
		void BeginPass();
		void EndPass();
		void Blit(uint32_t attachementIndex = 0);

		void Resize(uint32_t width, uint32_t height);
		void ClearTexture(uint32_t textureID, int clearID);

		inline Shared<FrameBuffer> const GetFrameBuffer() { return m_Framebuffer; }
		inline const Shared<FrameBuffer> &GetFrameBuffer() const { return m_Framebuffer; }

		inline Shared<Shader> const GetShader() { return m_Specifications.Program; }
		inline const Shared<Shader> &GetShader() const { return m_Specifications.Program; }

		inline const RenderPassSpecifications &GetSpecifications() const { return m_Specifications; }

	protected:
		RenderPassSpecifications m_Specifications;
		Shared<FrameBuffer> m_Framebuffer;
	};
}
