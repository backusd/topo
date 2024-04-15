#pragma once
#include "RootSignature.h"
#include "RootConstantBufferView.h"
#include "RenderPassLayer.h"
#include "ComputeLayer.h"



namespace topo
{
#ifdef DIRECTX12

struct ConstantBufferDescription
{
	unsigned int		Register;
	ConstantBufferBase* ConstantBuffer;
};
class RenderPassSignature
{
public:
	RenderPassSignature() {};
	RenderPassSignature(const std::vector<ConstantBufferDescription>& cbs) : ConstantBufferDescs(cbs) {}
	RenderPassSignature(std::vector<ConstantBufferDescription>&& cbs) : ConstantBufferDescs(std::move(cbs)) {}
	RenderPassSignature(const std::initializer_list<ConstantBufferDescription>& cbs) : ConstantBufferDescs(cbs) {}

	std::vector<ConstantBufferDescription> ConstantBufferDescs;
};

class RenderPass
{
public:
	RenderPass(std::shared_ptr<DeviceResources> deviceResources, const RenderPassSignature& signature) : m_rootSignature(nullptr)
	{
		std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameters(signature.ConstantBufferDescs.size());
		for (unsigned int iii = 0; iii < slotRootParameters.size(); ++iii)
		{
			slotRootParameters[iii].InitAsConstantBufferView(signature.ConstantBufferDescs[iii].Register);
		}

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(static_cast<unsigned int>(slotRootParameters.size()),
			slotRootParameters.data(), 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		m_rootSignature = std::make_shared<RootSignature>(deviceResources, rootSigDesc);

		// Create the Constant Buffer Views
		for (const ConstantBufferDescription& cbDesc : signature.ConstantBufferDescs)
		{
			RootConstantBufferView& cbv = EmplaceBackRootConstantBufferView(cbDesc.Register, cbDesc.ConstantBuffer);
			cbv.Update = cbDesc.ConstantBuffer->Update;
		}
	}


	inline RenderPass(std::shared_ptr<RootSignature> rootSig) noexcept :
		m_rootSignature(rootSig)
	{
		ASSERT(m_rootSignature != nullptr, "Root signature should not be nullptr");
	}
	inline RenderPass(std::shared_ptr<DeviceResources> deviceResources, const D3D12_ROOT_SIGNATURE_DESC& desc) :
		m_rootSignature(nullptr)
	{
		m_rootSignature = std::make_shared<RootSignature>(deviceResources, desc);
		ASSERT(m_rootSignature != nullptr, "Root signature should not be nullptr");
	}
	RenderPass(RenderPass&&) noexcept = default;
	RenderPass& operator=(RenderPass&&) noexcept = default;

	RenderPassLayer& EmplaceBackRenderPassLayer(std::shared_ptr<DeviceResources> deviceResources,
		MeshGroupBase* meshGroup,
		const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc,
		D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	{
		return m_renderPassLayers.emplace_back(deviceResources, meshGroup, desc, topology);
	}

	inline void Update(const Timer& timer, int frameIndex)
	{
		// Loop over the constant buffer views to update per-pass constants
		for (auto& rcbv : m_constantBufferViews)
			rcbv.Update(timer, frameIndex);
	}


	ND inline RootSignature* GetRootSignature() const noexcept { return m_rootSignature.get(); }

	// See here for article on 'deducing this' pattern: https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/
	template <class Self>
	ND constexpr auto&& GetRootConstantBufferViews(this Self&& self) noexcept { return std::forward<Self>(self).m_constantBufferViews; }
	template <class Self>
	ND constexpr auto&& GetRenderPassLayers(this Self&& self) noexcept { return std::forward<Self>(self).m_renderPassLayers; }
	template <class Self>
	ND constexpr auto&& GetComputeLayers(this Self&& self) noexcept { return std::forward<Self>(self).m_computeLayers; }



	inline void SetRootSignature(std::shared_ptr<RootSignature> rs) noexcept { m_rootSignature = rs; }


	constexpr void PushBackRootConstantBufferView(RootConstantBufferView&& rcbv) noexcept { m_constantBufferViews.push_back(std::move(rcbv)); }
	constexpr void PushBackRenderPassLayer(RenderPassLayer&& rpl) noexcept { m_renderPassLayers.push_back(std::move(rpl)); }
	constexpr void PushBackComputeLayer(ComputeLayer&& cl) noexcept { m_computeLayers.push_back(std::move(cl)); }

	constexpr RootConstantBufferView& EmplaceBackRootConstantBufferView(UINT rootParameterIndex, ConstantBufferBase* cb) noexcept { return m_constantBufferViews.emplace_back(rootParameterIndex, cb); }

	// Function pointers for Pre/Post-Work 
	// PreWork needs to return a bool: false -> signals early exit (i.e. do not make a Draw call for this layer)
	std::function<bool(RenderPass&, ID3D12GraphicsCommandList*)> PreWork = [](RenderPass&, ID3D12GraphicsCommandList*) { return true; };
	std::function<void(RenderPass&, ID3D12GraphicsCommandList*)> PostWork = [](RenderPass&, ID3D12GraphicsCommandList*) {};

private:
	// There is too much state to worry about copying (and expensive ?), so just delete copy operations until we find a good use case
	RenderPass(const RenderPass&) = delete;
	RenderPass& operator=(const RenderPass&) = delete;


	// Shared pointer because root signatures may be shared
	std::shared_ptr<RootSignature> m_rootSignature;

	// 0+ constant buffer views for per-pass constants
	std::vector<RootConstantBufferView> m_constantBufferViews;

	// 0+ render layers
	std::vector<RenderPassLayer> m_renderPassLayers;

	// 0+ render layers
	std::vector<ComputeLayer> m_computeLayers;




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