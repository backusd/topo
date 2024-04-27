#pragma once
#include "Camera.h"
#include "ConstantBuffer.h"
#include "InputLayout.h"
#include "Shader.h"
#include "RenderPass.h"

namespace topo
{
#ifdef DIRECTX12

class Renderer
{
public:
	inline Renderer(std::shared_ptr<DeviceResources> deviceResources = nullptr, const D3D12_VIEWPORT& viewport = {}, const D3D12_RECT& scissorRect = {}) noexcept :
		m_deviceResources(deviceResources),
		m_viewport(viewport),
		m_scissorRect(scissorRect)
	{}
	inline Renderer(Renderer&& rhs) noexcept :
		m_deviceResources(rhs.m_deviceResources),
		m_viewport(rhs.m_viewport),
		m_scissorRect(rhs.m_scissorRect),
		m_renderPasses(std::move(rhs.m_renderPasses))
	{}
	inline Renderer& operator=(Renderer&& rhs) noexcept
	{
		m_deviceResources = rhs.m_deviceResources;
		m_viewport = rhs.m_viewport;
		m_scissorRect = rhs.m_scissorRect;
		m_renderPasses = std::move(rhs.m_renderPasses);
		return *this;
	}

	inline void SetDeviceResources(std::shared_ptr<DeviceResources> deviceResources) noexcept { m_deviceResources = deviceResources; }

	void Update(const Timer& timer, int frameIndex);
	void Render(int frameIndex);

	ND constexpr RenderPass& GetRenderPass(unsigned int index) noexcept
	{
		ASSERT(index < m_renderPasses.size(), "index too large");
		return m_renderPasses[index];
	}

	constexpr void SetViewport(const D3D12_VIEWPORT& vp) noexcept { m_viewport = vp; }
	constexpr void SetScissorRect(const D3D12_RECT& rect) noexcept { m_scissorRect = rect; }

	constexpr void PushBackRenderPass(RenderPass&& pass) noexcept { m_renderPasses.push_back(std::move(pass)); }

	inline RenderPass& EmplaceBackRenderPass(const RenderPassSignature& signature) { return m_renderPasses.emplace_back(m_deviceResources, signature); }

private:
	// There is too much state to worry about copying (and expensive ?), so just delete copy operations until we find a good use case
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void RunComputeLayer(const ComputeLayer& layer, const Timer* timer, int frameIndex);

	std::shared_ptr<DeviceResources> m_deviceResources;

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

	std::vector<RenderPass> m_renderPasses;
};

#endif
}