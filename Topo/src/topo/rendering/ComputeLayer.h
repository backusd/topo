#pragma once
#include "RenderItem.h"
#include "RootSignature.h"

namespace topo
{
#ifdef TOPO_PLATFORM_WINDOWS

class ComputeLayer
{
public:
	inline ComputeLayer(std::shared_ptr<DeviceResources> deviceResources,
		std::shared_ptr<RootSignature> rootSig,
		const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc) :
		m_deviceResources(deviceResources),
		m_rootSignature(rootSig),
		m_pipelineState(nullptr),
		m_active(true)
	{
		ASSERT(m_rootSignature != nullptr, "Root Signature should not be nullptr");
		CreatePSO(desc);
	}
	inline ComputeLayer(std::shared_ptr<DeviceResources> deviceResources,
		const D3D12_ROOT_SIGNATURE_DESC& rootSigDesc,
		const D3D12_COMPUTE_PIPELINE_STATE_DESC& computePSODesc) :
		m_deviceResources(deviceResources),
		m_rootSignature(nullptr),
		m_pipelineState(nullptr),
		m_active(true)
	{
		m_rootSignature = std::make_shared<RootSignature>(m_deviceResources, rootSigDesc);
		CreatePSO(computePSODesc);
	}
	inline ComputeLayer(ComputeLayer&& rhs) noexcept :
		m_deviceResources(rhs.m_deviceResources),
		PreWork(std::move(rhs.PreWork)),
		PostWork(std::move(rhs.PostWork)),
		m_computeItems(std::move(rhs.m_computeItems)),
		m_pipelineState(rhs.m_pipelineState),
		m_active(rhs.m_active)
#ifndef TOPO_DIST
		, m_name(std::move(rhs.m_name))
#endif
	{}
	inline ComputeLayer& operator=(ComputeLayer&& rhs) noexcept
	{
		m_deviceResources = rhs.m_deviceResources;
		PreWork = std::move(rhs.PreWork);
		PostWork = std::move(rhs.PostWork);
		m_computeItems = std::move(rhs.m_computeItems);
		m_pipelineState = rhs.m_pipelineState;
		m_active = rhs.m_active;
#ifndef TOPO_DIST
		m_name = std::move(rhs.m_name);
#endif
		return *this;
	}
	~ComputeLayer() noexcept = default;

	inline void CreatePSO(const D3D12_COMPUTE_PIPELINE_STATE_DESC& desc)
	{
		GFX_THROW_INFO(m_deviceResources->GetDevice()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&m_pipelineState)));
	}

	inline void Update(const Timer& timer, int frameIndex)
	{
		for (ComputeItem& item : m_computeItems)
		{
			if (item.IsActive())
				item.Update(timer, frameIndex);
		}
	}

	constexpr void PushBackComputeItem(ComputeItem&& ci) noexcept { m_computeItems.push_back(std::move(ci)); }
	constexpr ComputeItem& EmplaceBackComputeItem(unsigned int threadGroupCountX = 1, unsigned int threadGroupCountY = 1, unsigned int threadGroupCountZ = 1) noexcept
	{
		return m_computeItems.emplace_back(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
	}

	// See here for article on 'deducing this' pattern: https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/
	template <class Self>
	ND constexpr auto&& GetComputeItems(this Self&& self) noexcept { return std::forward<Self>(self).m_computeItems; }
	ND inline ID3D12PipelineState* GetPSO() const noexcept { return m_pipelineState.Get(); }
	ND inline RootSignature* GetRootSignature() const noexcept { return m_rootSignature.get(); }
	ND constexpr bool IsActive() const noexcept { return m_active; }

	
	constexpr void SetActive(bool active) noexcept { m_active = active; }

	// PreWork needs to return a bool: false -> signals early exit (i.e. do not call Dispatch for this RenderLayer)
	// Also, because a ComputeLayer can be executed during the Update phase, it can get access to the Timer. However, 
	// if the ComputeLayer is execute during a RenderPass, then it will NOT have access to the timer and the timer 
	// parameter will be nullptr
	std::function<bool(const ComputeLayer&, ID3D12GraphicsCommandList*, const Timer*, int)> PreWork = [](const ComputeLayer&, ID3D12GraphicsCommandList*, const Timer*, int) { return true; };
	std::function<void(const ComputeLayer&, ID3D12GraphicsCommandList*, const Timer*, int)> PostWork = [](const ComputeLayer&, ID3D12GraphicsCommandList*, const Timer*, int) {};

private:
	// There is too much state to worry about copying, so just delete copy operations until we find a good use case
	ComputeLayer(const ComputeLayer&) noexcept = delete;
	ComputeLayer& operator=(const ComputeLayer&) noexcept = delete;

	std::shared_ptr<DeviceResources> m_deviceResources;

	// Shared pointer because root signatures may be shared
	std::shared_ptr<RootSignature>				m_rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
	std::vector<ComputeItem>					m_computeItems;
	bool										m_active;

// In DIST builds, we don't name the object
#ifndef TOPO_DIST
public:
	void SetDebugName(std::string_view name) noexcept { m_name = name; }
	ND const std::string& GetDebugName() const noexcept { return m_name; }
private:
	std::string m_name;
#endif
};

#endif
}