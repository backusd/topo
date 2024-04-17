#pragma once
#include "RootConstantBufferView.h"
#include "RootDescriptorTable.h"
#include "Texture.h"

namespace topo
{
#ifdef DIRECTX12

//
// RenderComputeItemBase
//
class RenderComputeItemBase
{
public:
	inline void Update(const Timer& timer, int frameIndex)
	{
		// Loop over the constant buffer views and descriptor tables to update them
		for (auto& rcbv : m_constantBufferViews)
			rcbv.GetConstantBuffer()->Update(timer, frameIndex);

		for (auto& dt : m_descriptorTables)
			dt.Update(&dt, timer, frameIndex);
	}

	constexpr void BindConstantBuffer(UINT rootParameterIndex, ConstantBufferBase* cb) noexcept { m_constantBufferViews.emplace_back(rootParameterIndex, cb); }
	constexpr void BindTexture(UINT rootParameterIndex, const Texture& texture) noexcept
	{
		// When binding a new Texture, we must make sure no two descriptor tables reference the same root parameter index
		// Therefore, if a table already exists for a given root parameter index, we add the new Texture to the existing
		// descriptor table
		for (auto& table : m_descriptorTables)
		{
			if (table.GetRootParameterIndex() == rootParameterIndex)
			{
				table.AddTexture(texture);
				return;
			}
		}

		// If we reach here, no existing table exists, so create a new one
		m_descriptorTables.emplace_back(rootParameterIndex, texture);
	}
	constexpr void BindTexture(UINT rootParameterIndex, Texture&& texture) noexcept
	{
		// When binding a new Texture, we must make sure no two descriptor tables reference the same root parameter index
		// Therefore, if a table already exists for a given root parameter index, we add the new Texture to the existing
		// descriptor table
		for (auto& table : m_descriptorTables)
		{
			if (table.GetRootParameterIndex() == rootParameterIndex)
			{
				table.AddTexture(std::move(texture));
				return;
			}
		}

		// If we reach here, no existing table exists, so create a new one
		m_descriptorTables.emplace_back(rootParameterIndex, std::move(texture));
	}
	constexpr void BindTextures(UINT rootParameterIndex, std::span<Texture> textures) noexcept
	{
		// When binding a new Texture, we must make sure no two descriptor tables reference the same root parameter index
		// Therefore, if a table already exists for a given root parameter index, we add the new Texture to the existing
		// descriptor table
		for (auto& table : m_descriptorTables)
		{
			if (table.GetRootParameterIndex() == rootParameterIndex)
			{
				table.AddTextures(textures);
				return;
			}
		}

		// If we reach here, no existing table exists, so create a new one
		m_descriptorTables.emplace_back(rootParameterIndex, textures);
	}
	constexpr void BindTextures(UINT rootParameterIndex, std::vector<Texture>&& textures) noexcept
	{
		// When binding a new Texture, we must make sure no two descriptor tables reference the same root parameter index
		// Therefore, if a table already exists for a given root parameter index, we add the new Texture to the existing
		// descriptor table
		for (auto& table : m_descriptorTables)
		{
			if (table.GetRootParameterIndex() == rootParameterIndex)
			{
				table.AddTextures(std::move(textures));
				return;
			}
		}

		// If we reach here, no existing table exists, so create a new one
		m_descriptorTables.emplace_back(rootParameterIndex, std::move(textures));
	}

//	constexpr void PushBackRootDescriptorTable(RootDescriptorTable&& rdt) noexcept { m_descriptorTables.push_back(std::move(rdt)); }
//	constexpr void PushBackRootDescriptorTable(const RootDescriptorTable& rdt) noexcept { m_descriptorTables.push_back(rdt); }
//	constexpr RootDescriptorTable& EmplaceBackRootDescriptorTable(UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE descriptorHandle) noexcept { return m_descriptorTables.emplace_back(rootParameterIndex, descriptorHandle); }

	// See here for article on 'deducing this' pattern: https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/
	template <class Self>
	ND constexpr auto&& GetRootConstantBufferViews(this Self&& self) noexcept { return std::forward<Self>(self).m_constantBufferViews; }
	template <class Self>
	ND constexpr auto&& GetRootDescriptorTables(this Self&& self) noexcept { return std::forward<Self>(self).m_descriptorTables; }

	ND constexpr bool IsActive() const noexcept { return m_active; }
	constexpr void SetActive(bool active) noexcept { m_active = active; }

protected:
	// 0+ constant buffer views for per-item constants
	std::vector<RootConstantBufferView> m_constantBufferViews;

	// 0+ descriptor tables for per-item resources
	std::vector<RootDescriptorTable> m_descriptorTables;

	bool m_active = true;


// In DIST builds, we don't name the object
#ifndef TOPO_DIST
public:
	void SetDebugName(std::string_view name) noexcept { m_name = name; }
	ND const std::string& GetDebugName() const noexcept { return m_name; }
private:
	std::string m_name;
#endif
};



//
// RenderItem
//
class RenderItem : public RenderComputeItemBase
{
public:
	constexpr RenderItem(unsigned int submeshIndex = 0, unsigned int instanceCount = 1) noexcept :
		m_submeshIndex(submeshIndex),
		m_instanceCount(instanceCount)
	{
		m_active = true;
	}
	constexpr RenderItem(RenderItem&& rhs) noexcept :
		RenderComputeItemBase(std::move(rhs)),
		m_submeshIndex(rhs.m_submeshIndex),
		m_instanceCount(rhs.m_instanceCount)
	{}
	constexpr RenderItem& operator=(RenderItem&& rhs) noexcept
	{
		RenderComputeItemBase::operator=(std::move(rhs));
		m_submeshIndex = rhs.m_submeshIndex;
		m_instanceCount = rhs.m_instanceCount;
		return *this;
	}

	ND constexpr unsigned int GetSubmeshIndex() const noexcept { return m_submeshIndex; }
	ND constexpr unsigned int GetInstanceCount() const noexcept { return m_instanceCount; }

	constexpr void SetSubmeshIndex(unsigned int index) noexcept { m_submeshIndex = index; }
	constexpr void SetInstanceCount(unsigned int count) noexcept { m_instanceCount = count; }

private:
	RenderItem(const RenderItem&) = delete;
	RenderItem& operator=(const RenderItem&) = delete;

	// The PSO will hold and bind the mesh-group for all of the render items it will render.
	// Here, we just need to keep track of which submesh index the render item references
	unsigned int m_submeshIndex;
	unsigned int m_instanceCount;
};



//
// ComputeItem
//
class ComputeItem : public RenderComputeItemBase
{
public:
	constexpr ComputeItem(unsigned int threadGroupCountX = 1,
		unsigned int threadGroupCountY = 1,
		unsigned int threadGroupCountZ = 1) noexcept :
		m_threadGroupCountX(threadGroupCountX),
		m_threadGroupCountY(threadGroupCountY),
		m_threadGroupCountZ(threadGroupCountZ)
	{}
	constexpr ComputeItem(ComputeItem&& rhs) noexcept :
		RenderComputeItemBase(std::move(rhs)),
		m_threadGroupCountX(rhs.m_threadGroupCountX),
		m_threadGroupCountY(rhs.m_threadGroupCountY),
		m_threadGroupCountZ(rhs.m_threadGroupCountZ)
	{}
	constexpr ComputeItem& operator=(ComputeItem&& rhs) noexcept
	{
		RenderComputeItemBase::operator=(std::move(rhs));
		m_threadGroupCountX = rhs.m_threadGroupCountX;
		m_threadGroupCountY = rhs.m_threadGroupCountY;
		m_threadGroupCountZ = rhs.m_threadGroupCountZ;
		return *this;
	}

	ND constexpr unsigned int GetThreadGroupCountX() const noexcept { return m_threadGroupCountX; }
	ND constexpr unsigned int GetThreadGroupCountY() const noexcept { return m_threadGroupCountY; }
	ND constexpr unsigned int GetThreadGroupCountZ() const noexcept { return m_threadGroupCountZ; }

	constexpr void SetThreadGroupCountX(unsigned int count) noexcept { m_threadGroupCountX = count; }
	constexpr void SetThreadGroupCountY(unsigned int count) noexcept { m_threadGroupCountY = count; }
	constexpr void SetThreadGroupCountZ(unsigned int count) noexcept { m_threadGroupCountZ = count; }

private:
	ComputeItem(const ComputeItem&) = delete;
	ComputeItem& operator=(const ComputeItem&) = delete;

	// Thread group counts to use when call Dispatch
	unsigned int m_threadGroupCountX = 1;
	unsigned int m_threadGroupCountY = 1;
	unsigned int m_threadGroupCountZ = 1;
};


#endif
}