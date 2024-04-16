#pragma once
#include "RootSignature.h"
#include "RootConstantBufferView.h"
#include "RenderPassLayer.h"
#include "ComputeLayer.h"
#include "Texture.h"
#include "SamplerData.h"
#include "PipelineStateDesc.h"

namespace topo
{
#ifdef DIRECTX12

struct TextureParameter
{
	unsigned int		BaseRegister = 0;
	unsigned int		Count = 1;
	unsigned int		RegisterSpace = 0;
};
struct SamplerParameter
{
	unsigned int Register;
	SamplerData* Desc;
};
struct ConstantBufferParameter
{
	unsigned int		Register;
	unsigned int		RegisterSpace = 0;
};
struct ShaderResourceViewParameter
{
	unsigned int		Register = 0;
	unsigned int		RegisterSpace = 0;
};
struct UnorderedAccessViewParameter
{
	unsigned int		Register = 0;
	unsigned int		RegisterSpace = 0;
};
class RenderPassSignature
{
	using ParameterType = std::variant<TextureParameter, ConstantBufferParameter, ShaderResourceViewParameter, UnorderedAccessViewParameter, SamplerParameter>;
public:
	RenderPassSignature() {};
	RenderPassSignature(const std::initializer_list<ParameterType>& rootParams) :
		m_rootParameters(rootParams)
	{}

	const std::vector<ParameterType>& GetSignatureParameters() const { return m_rootParameters; }

private:
	std::vector<ParameterType> m_rootParameters;
};

class RenderPass
{
public:
	RenderPass(std::shared_ptr<DeviceResources> deviceResources, const RenderPassSignature& signature) : m_rootSignature(nullptr)
	{
		const auto& signatureParams = signature.GetSignatureParameters();

		std::vector<CD3DX12_DESCRIPTOR_RANGE> ranges;
		std::vector<D3D12_STATIC_SAMPLER_DESC> samplerDescriptions;
		std::vector<CD3DX12_ROOT_PARAMETER> slotRootParameters;

		for (const auto& sigParam : signatureParams)
		{
			if (std::holds_alternative<TextureParameter>(sigParam))
			{
				ranges.clear();

				const TextureParameter& texDesc = std::get<TextureParameter>(sigParam);

				auto& range = ranges.emplace_back();
				range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, texDesc.Count, texDesc.BaseRegister, texDesc.RegisterSpace);

				auto& param = slotRootParameters.emplace_back();
				param.InitAsDescriptorTable(1, &range);
			}
			else if (std::holds_alternative<ConstantBufferParameter>(sigParam))
			{
				const ConstantBufferParameter& cbDesc = std::get<ConstantBufferParameter>(sigParam);

				auto& param = slotRootParameters.emplace_back(); 
				param.InitAsConstantBufferView(cbDesc.Register); 
			}
			else if (std::holds_alternative<ShaderResourceViewParameter>(sigParam))
			{
				const ShaderResourceViewParameter& desc = std::get<ShaderResourceViewParameter>(sigParam);
				auto& param = slotRootParameters.emplace_back();
				param.InitAsShaderResourceView(desc.Register, desc.RegisterSpace);
			}
			else if (std::holds_alternative<UnorderedAccessViewParameter>(sigParam))
			{
				const UnorderedAccessViewParameter& desc = std::get<UnorderedAccessViewParameter>(sigParam);
				auto& param = slotRootParameters.emplace_back();
				param.InitAsUnorderedAccessView(desc.Register, desc.RegisterSpace);
			}
			else if (std::holds_alternative<SamplerParameter>(sigParam))
			{
				const SamplerParameter& desc = std::get<SamplerParameter>(sigParam);
				const SamplerData& data = *(desc.Desc);
				
				D3D12_STATIC_SAMPLER_DESC d{};
				d.Filter = static_cast<D3D12_FILTER>(data.Filter); 
				d.AddressU = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(data.AddressU);
				d.AddressV = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(data.AddressV);
				d.AddressW = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(data.AddressW);
				d.MipLODBias = data.MipLODBias;
				d.MaxAnisotropy = data.MaxAnisotropy;
				d.ComparisonFunc = static_cast<D3D12_COMPARISON_FUNC>(data.ComparisonFunc);
				d.BorderColor = static_cast<D3D12_STATIC_BORDER_COLOR>(data.BorderColor);
				d.MinLOD = data.MinLOD;
				d.MaxLOD = data.MaxLOD;
				d.ShaderRegister = desc.Register;
				d.RegisterSpace = data.RegisterSpace;
				d.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(data.ShaderVisibility);
				
				samplerDescriptions.push_back(d);
			}
		}

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
			static_cast<unsigned int>(slotRootParameters.size()),
			slotRootParameters.data(), 
			static_cast<unsigned int>(samplerDescriptions.size()),
			samplerDescriptions.size() == 0 ? nullptr : samplerDescriptions.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		m_rootSignature = std::make_shared<RootSignature>(deviceResources, rootSigDesc);
		ASSERT(m_rootSignature != nullptr, "Something went wrong - Root signature should not be nullptr");
	}

	RenderPass(RenderPass&&) noexcept = default;
	RenderPass& operator=(RenderPass&&) noexcept = default;

	RenderPassLayer& EmplaceBackRenderPassLayer(std::shared_ptr<DeviceResources> deviceResources,
		MeshGroupBase* meshGroup,
		const PipelineStateDesc& desc) 
	{
		return m_renderPassLayers.emplace_back(deviceResources, meshGroup, desc);
	}

	inline void Bind(ID3D12GraphicsCommandList* commandList, int frameIndex)
	{
		ASSERT(commandList != nullptr, "Command List should not be nullptr");

		GFX_THROW_INFO_ONLY(commandList->SetGraphicsRootSignature(m_rootSignature->Get())); 
	
		for (const RootConstantBufferView& cbv : m_constantBufferViews)
		{
			GFX_THROW_INFO_ONLY(
				commandList->SetGraphicsRootConstantBufferView(
					cbv.GetRootParameterIndex(),
					cbv.GetConstantBuffer()->GetGPUVirtualAddress(frameIndex)
				)
			);
		}
	}
	inline void Update(const Timer& timer, int frameIndex)
	{
		// Loop over the constant buffer views to update per-pass constants
		for (auto& rcbv : m_constantBufferViews)
			rcbv.GetConstantBuffer()->Update(timer, frameIndex);
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

	constexpr void BindConstantBuffer(UINT rootParameterIndex, ConstantBufferBase* cb) noexcept { m_constantBufferViews.emplace_back(rootParameterIndex, cb); }

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
	void SetDebugName(std::string_view name) noexcept 
	{ 
		m_name = name; 

		SET_DEBUG_NAME_PTR(m_rootSignature, std::format("{0} - Root Signature", name)); 

		for (unsigned int iii = 0; iii < m_constantBufferViews.size(); ++iii)
			SET_DEBUG_NAME(m_constantBufferViews[iii], std::format("{0} - Constant Buffer View #{1}", name, iii));

		for (unsigned int iii = 0; iii < m_renderPassLayers.size(); ++iii)
			SET_DEBUG_NAME(m_renderPassLayers[iii], std::format("{0} - Render Pass Layer #{1}", name, iii));

		for (unsigned int iii = 0; iii < m_computeLayers.size(); ++iii)
			SET_DEBUG_NAME(m_computeLayers[iii], std::format("{0} - Compute Layer #{1}", name, iii));
	}
	ND const std::string& GetDebugName() const noexcept { return m_name; }
private:
	std::string m_name;
#endif
};

#endif
}