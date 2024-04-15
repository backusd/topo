#pragma once
#include "topo/DeviceResources.h"
#include "topo/Log.h"
#include "topo/utils/Concepts.h"
#include "topo/utils/Timer.h"

namespace topo
{
#ifdef DIRECTX12

// Defines a subrange of geometry in a MeshGeometry. This is for when multiple
// geometries are stored in one vertex and index buffer. It provides the offsets
// and data needed to draw a subset of geometry stores in the vertex and index buffers
struct SubmeshGeometryTEST
{
	SubmeshGeometryTEST() = default;
	SubmeshGeometryTEST(UINT indexCount, UINT startIndexLocation, INT baseVertexLocation, const DirectX::BoundingBox& boundingBox, const DirectX::BoundingSphere& boundingSphere) noexcept :
		IndexCount(indexCount), StartIndexLocation(startIndexLocation), BaseVertexLocation(baseVertexLocation),
		Bounds(boundingBox), Sphere(boundingSphere)
	{}

	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT  BaseVertexLocation = 0;

	// Bounding box and sphere of the geometry defined by this submesh. 
	DirectX::BoundingBox Bounds = {};
	DirectX::BoundingSphere Sphere = {};
};

struct MeshHandle
{

};

template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
class MeshGroupTEST;

// ===============================================================================================
// Mesh
//
template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
class MeshTEST
{
	friend class MeshGroupTEST<T>;

public:
	MeshTEST() noexcept = default;
	MeshTEST(std::vector<T>&& vertices, std::vector<std::uint16_t>&& indices) noexcept :
		m_vertices(std::move(vertices)), 
		m_indices(std::move(indices))
	{
		ASSERT(m_vertices.size() > 0, "No vertices");
		ASSERT(m_indices.size() > 0, "No indices");
		ComputeBounds();
	}
	MeshTEST(MeshTEST&&) noexcept = default;
	MeshTEST& operator=(MeshTEST&&) noexcept = default;

	void Set(std::vector<T>&& vertices, std::vector<std::uint16_t>&& indices)
	{
		m_vertices = std::move(vertices);
		m_indices = std::move(indices);
		ASSERT(m_vertices.size() > 0, "No vertices");
		ASSERT(m_indices.size() > 0, "No indices");
		ComputeBounds();
	}

private:
	// Meshes can be very large and expensive to copy, so just remove that ability until we find a good use case
	MeshTEST(const MeshTEST&) noexcept = delete;
	MeshTEST& operator=(const MeshTEST&) noexcept = delete;

	void ComputeBounds() noexcept;

	// System memory copies. 
	std::vector<T> m_vertices;
	std::vector<std::uint16_t> m_indices;

	DirectX::BoundingBox m_bounds;
	DirectX::BoundingSphere m_sphere;
};

template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
void MeshTEST<T>::ComputeBounds() noexcept
{
	using namespace DirectX;

	XMVECTOR vMin = DirectX::XMVectorSet(+FLT_MAX, +FLT_MAX, +FLT_MAX, 0.0f); 
	XMVECTOR vMax = DirectX::XMVectorSet(-FLT_MAX, -FLT_MAX, -FLT_MAX, 0.0f); 

	// Compute the min/max for the bounding box
	for (const T& v : m_vertices)
	{		
		XMFLOAT3 position = v.Position(); // This is guaranteed to work because we impose the concept: HasMemberFunctionPositionThatReturnsXMFLOAT3
		XMVECTOR p = XMLoadFloat3(&position);
		vMin = XMVectorMin(vMin, p);
		vMax = XMVectorMax(vMax, p);
	}

	// Compute the bounding box
	XMVECTOR center = 0.5f * (vMin + vMax);
	XMStoreFloat3(&m_bounds.Center, center);
	XMStoreFloat3(&m_bounds.Extents, 0.5f * (vMax - vMin));

	// Compute the bounding sphere
	XMVECTOR furthestPoint = center;

	for (const T& v : m_vertices)
	{
		XMFLOAT3 position = v.Position(); // This is guaranteed to work because we impose the concept: HasMemberFunctionPositionThatReturnsXMFLOAT3 
		XMVECTOR p = XMLoadFloat3(&position);
		if (XMVectorGetX(XMVector3Length(p - center)) > XMVectorGetX(XMVector3Length(furthestPoint - center)))
			furthestPoint = p;
	}

	m_sphere.Center = m_bounds.Center;
	m_sphere.Radius = XMVectorGetX(XMVector3Length(furthestPoint));
}

// ===============================================================================================
// MeshGroupBase
//
class MeshGroupBaseTEST
{
public:
	inline MeshGroupBaseTEST(std::shared_ptr<DeviceResources> deviceResources) noexcept :
		m_deviceResources(deviceResources)
	{}
	MeshGroupBaseTEST(MeshGroupBaseTEST&& rhs) noexcept;
	MeshGroupBaseTEST& operator=(MeshGroupBaseTEST&& rhs) noexcept;
	inline virtual ~MeshGroupBaseTEST() noexcept { CleanUp(); }

	inline void Bind(ID3D12GraphicsCommandList* commandList) const
	{
		GFX_THROW_INFO_ONLY(commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView));
		GFX_THROW_INFO_ONLY(commandList->IASetIndexBuffer(&m_indexBufferView));
	}

	ND inline const SubmeshGeometryTEST& GetSubmesh(unsigned int index) const noexcept { return m_submeshes[index]; }
	inline virtual void Update(int frameIndex) noexcept {}

protected:
	ND Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* initData, UINT64 byteSize) const;
	void UpdateBuffer(ID3D12Resource* buffer, const void* initData, UINT64 byteSize) const;
	inline void CleanUp() noexcept
	{
		// If the MeshGroup is being destructed because it was moved from, then we don't want to delete the resources
		// because they now belong to a new MeshGroup object. However, if the object simply went out of scope or was
		// intentially deleted, then the resources are no longer necessary and should be delayed deleted
		if (!m_movedFrom)
		{
			m_deviceResources->DelayedDelete(m_vertexBufferGPU);
			m_deviceResources->DelayedDelete(m_indexBufferGPU);
		}
	}

	std::shared_ptr<DeviceResources> m_deviceResources;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_indexBufferGPU = nullptr;

	// Keep track of views for the two buffers
	// NOTE: All values are dummy values except the DXGI_FORMAT value for the index buffer view
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = { 0, 0, 0 };
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView = { 0, 0, DXGI_FORMAT_R16_UINT };

	std::vector<SubmeshGeometryTEST> m_submeshes;

	bool m_movedFrom = false;

private:
	// There is too much state to worry about copying, so just delete copy operations until we find a good use case
	MeshGroupBaseTEST(const MeshGroupBaseTEST&) noexcept = delete;
	MeshGroupBaseTEST& operator=(const MeshGroupBaseTEST&) noexcept = delete;


	// In DIST builds, we don't name the object
#ifndef TOPO_DIST
public:
	void SetDebugName(std::string_view name) noexcept
	{
		m_name = name;
		std::string resource1Name = m_name + "-VertexBufferGPU";
		std::string resource2Name = m_name + "-IndexBufferGPU";
		m_vertexBufferGPU->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(resource1Name.size()), resource1Name.data());
		m_indexBufferGPU->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(resource2Name.size()), resource2Name.data());
	}
	ND const std::string& GetDebugName() const noexcept { return m_name; }
protected:
	std::string m_name;
#endif
};


// ============================================================================================
// MeshGroup 
//
template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
class MeshGroupTEST : public MeshGroupBaseTEST
{
public:
	MeshGroupTEST(std::shared_ptr<DeviceResources> deviceResources) noexcept :
		MeshGroupBaseTEST(deviceResources) 
	{}
	inline MeshGroupTEST(MeshGroupTEST&& rhs) noexcept :
		MeshGroupBaseTEST(std::move(rhs)), // See this SO post above calling std::move(rhs) but then proceding to use the rhs object: https://stackoverflow.com/questions/22977230/move-constructors-in-inheritance-hierarchy
		m_vertices(std::move(rhs.m_vertices)), 
		m_indices(std::move(rhs.m_indices))
	{}
	inline MeshGroupTEST& operator=(MeshGroupTEST&& rhs) noexcept
	{
		MeshGroupBaseTEST::operator=(std::move(rhs)); // See this SO post above calling std::move(rhs) but then proceding to use the rhs object: https://stackoverflow.com/questions/22977230/move-constructors-in-inheritance-hierarchy
		m_vertices = std::move(rhs.m_vertices);
		m_indices = std::move(rhs.m_indices);
		return *this;
	}
	inline virtual ~MeshGroupTEST() noexcept override { CleanUp(); }

	MeshHandle PushBack(const MeshTEST<T>& mesh);

private:
	// There is too much state to worry about copying, so just delete copy operations until we find a good use case
	MeshGroupTEST(const MeshGroupTEST&) noexcept = delete;
	MeshGroupTEST& operator=(const MeshGroupTEST&) noexcept = delete;

	// System memory copies. 
	std::vector<T> m_vertices;
	std::vector<std::uint16_t> m_indices;
};

template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
MeshHandle MeshGroupTEST<T>::PushBack(const MeshTEST<T>& mesh)
{
	using namespace DirectX;

	bool previouslyEmpty = m_vertices.size() == 0;

	// Create the new submesh structure for the mesh we are about to add
	m_submeshes.emplace_back(static_cast<UINT>(mesh.m_indices.size()), static_cast<UINT>(m_indices.size()),
							 static_cast<INT>(m_vertices.size()), mesh.m_bounds, mesh.m_sphere);

	// Reserve space and add new vertices
	m_vertices.reserve(m_vertices.size() + mesh.m_vertices.size());
	m_vertices.insert(m_vertices.end(), mesh.m_vertices.begin(), mesh.m_vertices.end());

	// Reserve space and add new indices
	m_indices.reserve(m_indices.size() + mesh.m_indices.size());
	m_indices.insert(m_indices.end(), mesh.m_indices.begin(), mesh.m_indices.end());
	
	// Compute the vertex/index buffer view data
	m_vertexBufferView.StrideInBytes = sizeof(T);
	m_vertexBufferView.SizeInBytes = static_cast<UINT>(m_vertices.size()) * sizeof(T);
	m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	m_indexBufferView.SizeInBytes = static_cast<UINT>(m_indices.size()) * sizeof(std::uint16_t);

	// If the buffer was previously empty, then we need to create them. Otherwise, we can just copy
	// in the new data to replace the old data
	if (previouslyEmpty)
	{
		m_vertexBufferGPU = CreateDefaultBuffer(m_vertices.data(), m_vertexBufferView.SizeInBytes);
		m_indexBufferGPU = CreateDefaultBuffer(m_indices.data(), m_indexBufferView.SizeInBytes);
	}
	else
	{
		UpdateBuffer(m_vertexBufferGPU.Get(), m_vertices.data(), m_vertexBufferView.SizeInBytes);
		UpdateBuffer(m_indexBufferGPU.Get(), m_indices.data(), m_indexBufferView.SizeInBytes);
	}

	// Get the buffer locations
	m_vertexBufferView.BufferLocation = m_vertexBufferGPU->GetGPUVirtualAddress();
	m_indexBufferView.BufferLocation = m_indexBufferGPU->GetGPUVirtualAddress();

#ifndef TOPO_DIST
	SetDebugName(m_name);
#endif

	return MeshHandle();
}

#endif
}