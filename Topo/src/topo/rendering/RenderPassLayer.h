#pragma once
#include "RenderItem.h"
#include "MeshGroup.h"
#include "PipelineStateDesc.h"
#include "Utility.h"

namespace topo
{
#ifdef DIRECTX12

class RenderPassLayer
{
public:
	inline RenderPassLayer(std::shared_ptr<DeviceResources> deviceResources,
							MeshGroupBase* meshGroup,
							const PipelineStateDesc& desc) :
		m_deviceResources(deviceResources),
		m_pipelineState(nullptr),
		m_meshes(meshGroup),
		m_stencilRef(std::nullopt),
		m_active(true)
	{
		ASSERT(meshGroup != nullptr, "MeshGroup must be set in the constructor");
		CreatePSO(desc);
	}
	RenderPassLayer(RenderPassLayer&& rhs) noexcept = default;
	RenderPassLayer& operator=(RenderPassLayer&& rhs) noexcept = default;


	inline void CreatePSO(const PipelineStateDesc& desc)
	{
#ifdef DIRECTX12
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = desc.ConvertToDirectX12();
		psoDesc.PrimitiveTopologyType = static_cast<D3D12_PRIMITIVE_TOPOLOGY_TYPE>(m_meshes->GetTopologyType());
		GFX_THROW_INFO(
			m_deviceResources->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState))
		);
#else
#error Only supporting DirectX12
#endif
	}

	inline void Update(const Timer& timer, int frameIndex)
	{
		for (RenderItem& item : m_renderItems)
		{
			if (item.IsActive())
				item.Update(timer, frameIndex);
		}

		// This is a virtual method that only does anything for DynamicMesh
		m_meshes->Update(frameIndex);
	}

	constexpr void PushBackRenderItem(RenderItem&& ri) noexcept { m_renderItems.push_back(std::move(ri)); }
	constexpr RenderItem& EmplaceBackRenderItem(unsigned int submeshIndex = 0, unsigned int instanceCount = 1) noexcept { return m_renderItems.emplace_back(submeshIndex, instanceCount); }

	// See here for article on 'deducing this' pattern: https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/
	template <class Self>
	ND constexpr auto&& GetRenderItems(this Self&& self) noexcept { return std::forward<Self>(self).m_renderItems; }
	template <class Self>
	ND constexpr auto&& GetRenderItem(this Self&& self, unsigned int index) noexcept { return std::forward<Self>(self).m_renderItems[index]; }


	ND inline ID3D12PipelineState* GetPSO() const noexcept { return m_pipelineState.Get(); }
	ND inline MeshGroupBase* GetMeshGroup() const noexcept { return m_meshes; }
	ND constexpr bool IsActive() const noexcept { return m_active; }
	ND constexpr std::optional<unsigned int> GetStencilRef() const noexcept { return m_stencilRef; }

	constexpr void SetActive(bool active) noexcept { m_active = active; }

	constexpr void SetStencilRef(unsigned int value) noexcept { m_stencilRef = value; }
	constexpr void SetStencilRef(std::optional<unsigned int> value) noexcept { m_stencilRef = value; }


	// PreWork needs to return a bool: false -> signals early exit (i.e. do not make a Draw call for this layer)
	std::function<bool(const RenderPassLayer&, ID3D12GraphicsCommandList*)> PreWork = [](const RenderPassLayer&, ID3D12GraphicsCommandList*) { return true; };
	std::function<void(const RenderPassLayer&, ID3D12GraphicsCommandList*)> PostWork = [](const RenderPassLayer&, ID3D12GraphicsCommandList*) {};

private:
	// There is too much state to worry about copying (and expensive ?), so just delete copy operations until we find a good use case
	RenderPassLayer(const RenderPassLayer&) noexcept = delete;
	RenderPassLayer& operator=(const RenderPassLayer&) noexcept = delete;

	std::shared_ptr<DeviceResources> m_deviceResources;

	std::vector<RenderItem>						m_renderItems;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
	MeshGroupBase*								m_meshes;		// raw pointer because it just needs to reference/read the mesh data. A different class should own the meshes
	bool										m_active;
	std::optional<unsigned int>					m_stencilRef;





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