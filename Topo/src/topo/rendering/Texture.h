#pragma once

#include "topo/DeviceResources.h"



namespace topo
{
class TextureBackingObject
{
	friend class Texture;
public:
	TextureBackingObject(std::shared_ptr<DeviceResources> deviceResources, std::string_view filename);
	TextureBackingObject(const TextureBackingObject& rhs) = default;
	TextureBackingObject(TextureBackingObject&& rhs) noexcept = default;
	TextureBackingObject& operator=(const TextureBackingObject& rhs) = default;
	TextureBackingObject& operator=(TextureBackingObject&& rhs) noexcept = default;
	inline ~TextureBackingObject() noexcept
	{
		m_deviceResources->GetDescriptorVector()->ReleaseAt(m_srvDescriptorIndex);
		m_deviceResources->DelayedDelete(m_textureResource);
	}

	ND inline D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandle() const noexcept 
	{
		return m_deviceResources->GetDescriptorVector()->GetGPUHandleAt(m_srvDescriptorIndex);
	}

	ND inline const std::string& Filename() const noexcept { return m_filename; }

private:
	std::shared_ptr<DeviceResources>		m_deviceResources;
	std::string								m_filename;
	unsigned int							m_srvDescriptorIndex = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource>	m_textureResource = nullptr;
};



class AssetManager;

class Texture
{
	friend class AssetManager;

public:
	inline Texture() noexcept :
		m_texture(nullptr)
	{}
	inline Texture(TextureBackingObject* texture) :
		m_texture(texture)
	{}
	Texture(const Texture& rhs);
	Texture(Texture&& rhs) noexcept;
	Texture& operator=(const Texture& rhs);
	Texture& operator=(Texture&& rhs) noexcept;
	~Texture();

	ND inline D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandle() const noexcept
	{
		return m_texture->GetSRVHandle();
	}

private:
	TextureBackingObject* m_texture;
};

}