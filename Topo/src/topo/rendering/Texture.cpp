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

	SET_DEBUG_NAME_PTR(this, std::format("{0} {1}", m_deviceResources->Name(), m_filename));
}



Texture::Texture(const Texture& rhs) :
	m_texture(rhs.m_texture)
{
	if (m_texture != nullptr)
	{
		std::string key = std::format("{0} {1}", m_texture->m_deviceResources->Name(), m_texture->m_filename);
		AssetManager::TextureIncrementCount(key);
	}
}
Texture::Texture(Texture&& rhs) noexcept :
	m_texture(rhs.m_texture)
{
	// Set rhs texture to nullptr on the moved-from object so that it won't decrement the ref count
	rhs.m_texture = nullptr;
}
Texture& Texture::operator=(const Texture& rhs)
{
	m_texture = rhs.m_texture;

	if (m_texture != nullptr)
	{
		std::string key = std::format("{0} {1}", m_texture->m_deviceResources->Name(), m_texture->m_filename);
		AssetManager::TextureIncrementCount(key);
	}
	
	return *this;
}
Texture& Texture::operator=(Texture&& rhs) noexcept
{
	m_texture = rhs.m_texture;

	// Set rhs texture to nullptr on the moved-from object so that it won't decrement the ref count
	rhs.m_texture = nullptr;

	return *this;
}
Texture::~Texture()
{
	if (m_texture != nullptr)
	{
		std::string key = std::format("{0} {1}", m_texture->m_deviceResources->Name(), m_texture->m_filename);
		AssetManager::TextureDecrementCount(key);
	}
}


}