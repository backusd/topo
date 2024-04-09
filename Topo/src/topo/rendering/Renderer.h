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
	Renderer(std::shared_ptr<DeviceResources> deviceResources, D3D12_VIEWPORT& viewport, D3D12_RECT& scissorRect) noexcept;
	inline Renderer(Renderer&& rhs) noexcept :
		m_deviceResources(rhs.m_deviceResources),
		m_camera(std::move(rhs.m_camera)),
		m_viewport(rhs.m_viewport),
		m_scissorRect(rhs.m_scissorRect),
		m_renderPasses(std::move(rhs.m_renderPasses))
	{}
	inline Renderer& operator=(Renderer&& rhs) noexcept
	{
		m_deviceResources = rhs.m_deviceResources;
		m_camera = std::move(rhs.m_camera);
		m_viewport = rhs.m_viewport;
		m_scissorRect = rhs.m_scissorRect;
		m_renderPasses = std::move(rhs.m_renderPasses);
		return *this;
	}
	inline ~Renderer() noexcept = default;

	void Update(const Timer& timer, int frameIndex);
	void Render(int frameIndex);

	ND constexpr Camera& GetCamera() noexcept { return m_camera; }
	ND constexpr const Camera& GetCamera() const noexcept { return m_camera; }
	ND constexpr RenderPass& GetRenderPass(unsigned int index) noexcept
	{
		ASSERT(index < m_renderPasses.size(), "index too large");
		return m_renderPasses[index];
	}

	constexpr void SetViewport(D3D12_VIEWPORT& vp) noexcept { m_viewport = vp; }
	constexpr void SetScissorRect(D3D12_RECT& rect) noexcept { m_scissorRect = rect; }

	constexpr void PushBackRenderPass(RenderPass&& pass) noexcept { m_renderPasses.push_back(std::move(pass)); }

	inline RenderPass& EmplaceBackRenderPass(std::shared_ptr<RootSignature> rootSig) noexcept { return m_renderPasses.emplace_back(rootSig); }
	inline RenderPass& EmplaceBackRenderPass(std::shared_ptr<DeviceResources> deviceResources, const D3D12_ROOT_SIGNATURE_DESC& desc) noexcept { return m_renderPasses.emplace_back(deviceResources, desc); }

private:
	// There is too much state to worry about copying (and expensive ?), so just delete copy operations until we find a good use case
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void RunComputeLayer(const ComputeLayer& layer, const Timer* timer, int frameIndex);

	std::shared_ptr<DeviceResources> m_deviceResources;

	// Note: we do NOT allow camera to be a reference to a Camera object controlled by
	// the application. The reason being is that we must enforce each Renderer instance to
	// have its own camera (no sharing a camera). The application is responsible for calling
	// GetCamera() and updating its position/orientation when necessary
	Camera m_camera;

	// Have the viewport and scissor rect be controlled by the application. We use references
	// here because neither of these should ever be allowed to be null
	D3D12_VIEWPORT& m_viewport;
	D3D12_RECT& m_scissorRect;

	std::vector<RenderPass> m_renderPasses;
};

#endif
}