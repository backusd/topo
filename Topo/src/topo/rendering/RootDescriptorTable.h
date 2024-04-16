#pragma once
#include "topo/DeviceResources.h"
#include "topo/utils/Timer.h"
#include "Texture.h"

namespace topo
{
#ifdef DIRECTX12

class RootDescriptorTable
{
public:
	inline RootDescriptorTable(UINT rootParameterIndex, const Texture& texture) noexcept :
		m_rootParameterIndex(rootParameterIndex),
		m_texture(texture)
	{}
	inline RootDescriptorTable(UINT rootParameterIndex, Texture&& texture) noexcept :
		m_rootParameterIndex(rootParameterIndex),
		m_texture(std::move(texture))
	{}
	RootDescriptorTable(const RootDescriptorTable&) noexcept = default;
	RootDescriptorTable(RootDescriptorTable&&) noexcept = default;
	RootDescriptorTable& operator=(const RootDescriptorTable&) noexcept = default;
	RootDescriptorTable& operator=(RootDescriptorTable&&) noexcept = default;

	std::function<void(RootDescriptorTable*, const Timer&, int)> Update = [](RootDescriptorTable*, const Timer&, int) {};

	ND constexpr UINT GetRootParameterIndex() const noexcept { return m_rootParameterIndex; }
	ND inline D3D12_GPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const noexcept { return m_texture.GetSRVHandle(); }

private:
	UINT m_rootParameterIndex;
	Texture m_texture;





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