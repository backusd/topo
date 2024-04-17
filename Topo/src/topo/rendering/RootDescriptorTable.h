#pragma once
#include "topo/DeviceResources.h"
#include "topo/utils/Timer.h"
#include "Texture.h"

#ifdef TOPO_DEBUG
#define SRV_CONTINUITY_CHECK() SRVContinuityCheck()
#else
#define SRV_CONTINUITY_CHECK()
#endif

namespace topo
{
#ifdef DIRECTX12

class RootDescriptorTable
{
public:
	inline RootDescriptorTable(UINT rootParameterIndex, const Texture& texture) noexcept :
		m_rootParameterIndex(rootParameterIndex),
		m_textures{ texture },
		m_srvIndices(),
		m_descriptorVector(nullptr) // Wait to assign it until after we have done a validity check
	{
		ASSERT(texture.HasData(), "Invalid to use a Texture without a valid pointer to a backing object");

		m_descriptorVector = texture.GetDeviceResources()->GetDescriptorVector();
		ASSERT(m_descriptorVector != nullptr, "Should not be nullptr");

		CreateSRV(m_textures[0]);
		ASSERT(m_textures.size() == m_srvIndices.size(), "Sizes should match");
	}
	inline RootDescriptorTable(UINT rootParameterIndex, Texture&& texture) noexcept :
		m_rootParameterIndex(rootParameterIndex),
		m_textures{ std::move(texture) },
		m_srvIndices(),
		m_descriptorVector(nullptr) // Wait to assign it until after we have done a validity check
	{
		ASSERT(m_textures[0].HasData(), "Invalid to use a Texture without a valid pointer to a backing object");

		m_descriptorVector = m_textures[0].GetDeviceResources()->GetDescriptorVector();
		ASSERT(m_descriptorVector != nullptr, "Should not be nullptr");

		CreateSRV(m_textures[0]);
		ASSERT(m_textures.size() == m_srvIndices.size(), "Sizes should match");
	}
	inline RootDescriptorTable(UINT rootParameterIndex, std::span<Texture> textures) noexcept :
		m_rootParameterIndex(rootParameterIndex),
		m_textures(),
		m_srvIndices(),
		m_descriptorVector(nullptr) // Wait to assign it until after we have done a validity check
	{
		ASSERT(textures.size() > 0, "Must pass in at least one Texture");

		m_textures.assign_range(textures);

		m_descriptorVector = m_textures[0].GetDeviceResources()->GetDescriptorVector();
		ASSERT(m_descriptorVector != nullptr, "Should not be nullptr");
		
		CreateSRV(m_textures);
		ASSERT(m_textures.size() == m_srvIndices.size(), "Sizes should match");

#ifdef TOPO_DEBUG
		for (const auto& texture : m_textures)
			ASSERT(texture.HasData(), "Invalid to use a Texture without a valid pointer to a backing object");
#endif

		SRV_CONTINUITY_CHECK(); 
	}
	inline RootDescriptorTable(UINT rootParameterIndex, std::vector<Texture>&& textures) noexcept :
		m_rootParameterIndex(rootParameterIndex),
		m_textures(std::move(textures)),
		m_srvIndices(),
		m_descriptorVector(nullptr) // Wait to assign it until after we have done a validity check
	{
		ASSERT(m_textures.size() > 0, "Must pass in at least one Texture");

		m_descriptorVector = m_textures[0].GetDeviceResources()->GetDescriptorVector();
		ASSERT(m_descriptorVector != nullptr, "Should not be nullptr");

		CreateSRV(m_textures);
		ASSERT(m_textures.size() == m_srvIndices.size(), "Sizes should match");

#ifdef TOPO_DEBUG
		for (const auto& texture : m_textures)
			ASSERT(texture.HasData(), "Invalid to use a Texture without a valid pointer to a backing object");
#endif

		SRV_CONTINUITY_CHECK();
	}
	RootDescriptorTable(const RootDescriptorTable&) noexcept = default;
	RootDescriptorTable(RootDescriptorTable&&) noexcept = default;
	RootDescriptorTable& operator=(const RootDescriptorTable&) noexcept = default;
	RootDescriptorTable& operator=(RootDescriptorTable&&) noexcept = default;
	inline ~RootDescriptorTable() noexcept { m_descriptorVector->ReleaseAt(m_srvIndices); }

	inline void AddTexture(const Texture& texture) noexcept
	{
		ASSERT(texture.HasData(), "Invalid to use a Texture without a valid pointer to a backing object");

		m_textures.push_back(texture);
		CreateSRV(m_textures.back());
		ASSERT(m_textures.size() == m_srvIndices.size(), "Sizes should match");

		SRV_CONTINUITY_CHECK();
	}
	inline void AddTexture(Texture&& texture) noexcept
	{
		m_textures.push_back(std::move(texture));
		ASSERT(m_textures.back().HasData(), "Invalid to use a Texture without a valid pointer to a backing object");
		CreateSRV(m_textures.back());
		ASSERT(m_textures.size() == m_srvIndices.size(), "Sizes should match");

		SRV_CONTINUITY_CHECK();
	}
	inline void AddTextures(std::span<Texture> textures) noexcept
	{
		ASSERT(textures.size() > 0, "Must pass in at least one Texture");

		CreateSRV(textures);
		m_textures.append_range(textures);
		ASSERT(m_textures.size() == m_srvIndices.size(), "Sizes should match");

		SRV_CONTINUITY_CHECK();
	}
	inline void AddTextures(std::vector<Texture>&& textures) noexcept
	{
		ASSERT(textures.size() > 0, "Must pass in at least one Texture");

		CreateSRV(textures); 
		m_textures.append_range(std::move(textures));
		ASSERT(m_textures.size() == m_srvIndices.size(), "Sizes should match");

		SRV_CONTINUITY_CHECK();
	}

	std::function<void(RootDescriptorTable*, const Timer&, int)> Update = [](RootDescriptorTable*, const Timer&, int) {};

	ND constexpr UINT GetRootParameterIndex() const noexcept { return m_rootParameterIndex; }
	ND inline D3D12_GPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const noexcept 
	{ 		
		return m_descriptorVector->GetGPUHandleAt(m_srvIndices[0]);
	}

private:
	inline void SRVContinuityCheck()
	{
		for (unsigned int iii = 1; iii < m_srvIndices.size(); ++iii)
			ASSERT(m_srvIndices[iii - 1] == m_srvIndices[iii] - 1, "Textures must have SRV's that are continuous in the descriptor heap");
	}
	void CreateSRV(const Texture& texture);
	void CreateSRV(std::span<Texture> texture);

	UINT m_rootParameterIndex;

	// Cache a pointer to the descriptor vector
	DescriptorVector* m_descriptorVector;
	
	// We use a vector of textures here because of the case where we specify a TextureParameter in the
	// RenderPassSignature that declares a count of more than 1 Texture. In that scenario, there is only
	// a single root parameter index that applies to multiple Textures. However, during rendering, we
	// don't bind each Texture's SRV individually, rather, we only bind the first Shader Resource View and it
	// is assumed that all additional Shader Resource Views are continuous in the descriptor heap. So holding
	// a vector here allows us to do a runtime check that this is in fact the case. ALSO, Texture is a managed,
	// ref-counted class, so we must hold a reference to it somewhere, otherwise, the underlying Texture data
	// may be released prematurely.
	std::vector<Texture> m_textures;

	// Keep a vector of the SRV index for each Texture. By having the descriptor table own the SRV, we 
	// can reuse any Texture with different RenderItems, which will create new SRV's, which means we
	// won't have to ever worry about the SRV's not being continuous in the descriptor heap
	std::vector<unsigned int> m_srvIndices; 




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