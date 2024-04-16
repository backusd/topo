#include "pch.h"
#include "Texture.h"
#include "topo/utils/DDSTextureLoader.h"
#include "topo/utils/String.h"
#include "AssetManager.h"

namespace topo
{
TextureBackingObject::TextureBackingObject(std::shared_ptr<DeviceResources> deviceResources, std::string_view filename) :
	m_deviceResources(deviceResources),
	m_filename(filename)
{
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadHeap = nullptr;

	// Load the texture from file
	std::wstring w_filename = s2ws(m_filename);
	GFX_THROW_INFO(
		DirectX::CreateDDSTextureFromFile12(
			m_deviceResources->GetDevice(),
			m_deviceResources->GetCommandList(),
			w_filename.c_str(),
			m_textureResource,
			uploadHeap
		)
	);

	// Do a delayed delete on the upload heap
	m_deviceResources->DelayedDelete(uploadHeap);

	// Create the SRV descriptor for the texture
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = m_textureResource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create a SRV and keep track of the index needed to look up this descriptor
	DescriptorVector* dv = m_deviceResources->GetDescriptorVector();
	m_srvDescriptorIndex = dv->EmplaceBackShaderResourceView(m_textureResource.Get(), &srvDesc);
}



Texture::Texture(const Texture& rhs) :
	m_texture(rhs.m_texture)
{
	AssetManager::TextureIncrementCount(m_texture->Filename());
}
Texture::Texture(Texture&& rhs) noexcept :
	m_texture(rhs.m_texture)
{
	AssetManager::TextureIncrementCount(m_texture->Filename());
}
Texture& Texture::operator=(const Texture& rhs)
{
	m_texture = rhs.m_texture;
	AssetManager::TextureIncrementCount(m_texture->Filename());
	return *this;
}
Texture& Texture::operator=(Texture&& rhs) noexcept
{
	m_texture = rhs.m_texture;
	AssetManager::TextureIncrementCount(m_texture->Filename());
	return *this;
}
Texture::~Texture()
{
	// When a shader has been moved from, m_key will no longer hold any data
	// Therefore, we should only decrement the count if m_key still holds a value
	AssetManager::TextureDecrementCount(m_texture->Filename());
}


}