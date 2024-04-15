#pragma once

#include "topo/DeviceResources.h"



namespace topo
{
class Texture
{
public:
	Texture(std::shared_ptr<DeviceResources> deviceResources, std::string_view filename);

	ND inline D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandle() const noexcept 
	{
		return m_deviceResources->GetDescriptorVector()->GetGPUHandleAt(m_srvDescriptorIndex);
	}

private:
	std::shared_ptr<DeviceResources>		m_deviceResources;
	unsigned int							m_srvDescriptorIndex = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource>	m_textureResource = nullptr;
};
}