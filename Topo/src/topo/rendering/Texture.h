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
	inline ~TextureBackingObject() noexcept { m_deviceResources->DelayedDelete(m_textureResource); }

	ND inline const std::string& Filename() const noexcept { return m_filename; }

private:
	std::shared_ptr<DeviceResources>		m_deviceResources;
	std::string								m_filename;
	Microsoft::WRL::ComPtr<ID3D12Resource>	m_textureResource = nullptr;


// In DIST builds, we don't name the object
#ifndef TOPO_DIST
public:
	void SetDebugName(std::string_view name) noexcept
	{
		m_textureResource->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(name.size()), name.data());
	}
	ND std::string GetDebugName() const noexcept
	{
		char name[64] = {};
		UINT size = sizeof(name);
		m_textureResource->GetPrivateData(WKPDID_D3DDebugObjectName, &size, name);
		return name;
	}
#endif
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

	void SetBackingObject(TextureBackingObject* obj)
	{
		ASSERT(m_texture == nullptr, "Invalid to manually reset backing object.");
		m_texture = obj;
	}
	ND constexpr bool HasData() const noexcept { return m_texture != nullptr; }
	ND inline DXGI_FORMAT GetFormat() const noexcept { return m_texture->m_textureResource->GetDesc().Format; }
	ND inline std::shared_ptr<DeviceResources> GetDeviceResources() const noexcept { return m_texture->m_deviceResources; }
	ND inline ID3D12Resource* GetResource() const noexcept { return m_texture->m_textureResource.Get(); }

private:
	TextureBackingObject* m_texture;



// In DIST builds, we don't name the object
#ifndef TOPO_DIST
public:
	void SetDebugName(std::string_view name) noexcept { m_texture->SetDebugName(name); }
	ND std::string GetDebugName() const noexcept { return m_texture->GetDebugName(); }
#endif
};

}