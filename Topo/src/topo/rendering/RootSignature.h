#pragma once
#include "topo/DeviceResources.h"
#include "topo/Log.h"


namespace topo
{
#ifdef DIRECTX12

class RootSignature
{
public:
	inline RootSignature(std::shared_ptr<DeviceResources> deviceResources, const D3D12_ROOT_SIGNATURE_DESC& desc) :
		m_deviceResources(deviceResources), m_rootSignature(nullptr)
	{
		// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
		Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT _hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
		{
			LOG_ERROR("D3D12SerializeRootSignature() failed with message: {}", (char*)errorBlob->GetBufferPointer());
		}
		if (FAILED(_hr))
		{
			INFOMAN
				throw GFX_EXCEPT(_hr);
		}

		GFX_THROW_INFO(
			m_deviceResources->GetDevice()->CreateRootSignature(
				0,
				serializedRootSig->GetBufferPointer(),
				serializedRootSig->GetBufferSize(),
				IID_PPV_ARGS(&m_rootSignature)
			)
		);
	}
	RootSignature(const RootSignature&) noexcept = default;
	RootSignature(RootSignature&&) noexcept = default;
	RootSignature& operator=(const RootSignature&) noexcept = default;
	RootSignature& operator=(RootSignature&&) noexcept = default;
	~RootSignature() noexcept {}

	ND ID3D12RootSignature* Get() const noexcept { return m_rootSignature.Get(); }

private:
	std::shared_ptr<DeviceResources> m_deviceResources;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;


// In DIST builds, we don't name the object
#ifndef TOPO_DIST
public:
	void SetDebugName(std::string_view name) noexcept
	{
		m_rootSignature->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.size()), name.data());
	}
	ND std::string GetDebugName() const noexcept
	{
		char name[64] = {};
		UINT size = sizeof(name);
		m_rootSignature->GetPrivateData(WKPDID_D3DDebugObjectName, &size, name);
		return name;
	}
#endif
};

#endif
}