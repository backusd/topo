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
struct MeshDescriptor
{
	MeshDescriptor() = default;
	MeshDescriptor(UINT indexCount, UINT startIndexLocation, INT baseVertexLocation, UINT vertexCount, const DirectX::BoundingBox& boundingBox, const DirectX::BoundingSphere& boundingSphere) noexcept :
		IndexCount(indexCount), StartIndexLocation(startIndexLocation), BaseVertexLocation(baseVertexLocation),
		VertexCount(vertexCount), Bounds(boundingBox), Sphere(boundingSphere)
	{}
	MeshDescriptor(const MeshDescriptor&) = default;
	MeshDescriptor(MeshDescriptor&&) noexcept = default;
	MeshDescriptor& operator=(const MeshDescriptor&) = default;
	MeshDescriptor& operator=(MeshDescriptor&&) noexcept = default;

	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT  BaseVertexLocation = 0;
	UINT VertexCount = 0;

	// Bounding box and sphere of the geometry defined by this submesh. 
	DirectX::BoundingBox Bounds = {};
	DirectX::BoundingSphere Sphere = {};
};

template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
class MeshGroup;

// ===============================================================================================
// Mesh
//
template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
class Mesh
{
	friend class MeshGroup<T>;

public:
	Mesh() noexcept = default;
	Mesh(std::vector<T>&& vertices, std::vector<std::uint16_t>&& indices) noexcept :
		m_vertices(std::move(vertices)),
		m_indices(std::move(indices))
	{
		ASSERT(m_vertices.size() > 0, "No vertices");
		ASSERT(m_indices.size() > 0, "No indices");
		ComputeBounds();
	}
	Mesh(Mesh&&) noexcept = default;
	Mesh& operator=(Mesh&&) noexcept = default;

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
	Mesh(const Mesh&) noexcept = delete;
	Mesh& operator=(const Mesh&) noexcept = delete;

	void ComputeBounds() noexcept;

	// System memory copies. 
	std::vector<T> m_vertices;
	std::vector<std::uint16_t> m_indices;

	DirectX::BoundingBox m_bounds;
	DirectX::BoundingSphere m_sphere;
};

template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
void Mesh<T>::ComputeBounds() noexcept
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
class MeshGroupBase
{
public:
	inline MeshGroupBase(std::shared_ptr<DeviceResources> deviceResources, PRIMITIVE_TOPOLOGY topology) noexcept :
		m_deviceResources(deviceResources),
		m_topology(topology)
	{}
	MeshGroupBase(MeshGroupBase&& rhs) noexcept;
	MeshGroupBase& operator=(MeshGroupBase&& rhs) noexcept;
	MeshGroupBase(const MeshGroupBase& rhs) noexcept :
		m_deviceResources(rhs.m_deviceResources),
		m_vertexBufferGPU(nullptr),
		m_indexBufferGPU(nullptr),
		m_vertexBufferView(rhs.m_vertexBufferView),
		m_indexBufferView(rhs.m_indexBufferView),
		m_submeshes(rhs.m_submeshes),
		m_topology(rhs.m_topology),
		m_movedFrom(false)
	{}
	MeshGroupBase& operator=(const MeshGroupBase& rhs) noexcept
	{
		m_deviceResources = rhs.m_deviceResources;
		m_vertexBufferGPU = nullptr;
		m_indexBufferGPU = nullptr;
		m_vertexBufferView = rhs.m_vertexBufferView;
		m_indexBufferView = rhs.m_indexBufferView;
		m_submeshes = rhs.m_submeshes;
		m_topology = rhs.m_topology;
		m_movedFrom = false;
		return *this;
	}
	inline virtual ~MeshGroupBase() noexcept { CleanUp(); }

	inline void Bind(ID3D12GraphicsCommandList* commandList) const
	{
		GFX_THROW_INFO_ONLY(commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView));
		GFX_THROW_INFO_ONLY(commandList->IASetIndexBuffer(&m_indexBufferView));
		GFX_THROW_INFO_ONLY(
			commandList->IASetPrimitiveTopology(static_cast<D3D12_PRIMITIVE_TOPOLOGY>(m_topology))
		);
	}
	ND constexpr PRIMITIVE_TOPOLOGY GetTopology() const noexcept { return m_topology; }
	ND constexpr PRIMITIVE_TOPOLOGY_TYPE GetTopologyType() const noexcept { return DeducePrimitiveTopologyType(m_topology); }

	ND inline const MeshDescriptor& GetSubmesh(unsigned int index) const noexcept { return m_submeshes[index]; }
	inline virtual void Update(int frameIndex) noexcept {}

protected:
	ND Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* initData, UINT64 byteSize) const;
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

	std::vector<MeshDescriptor> m_submeshes;
	PRIMITIVE_TOPOLOGY m_topology;

	bool m_movedFrom = false;



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
class MeshGroup : public MeshGroupBase
{
public:
	MeshGroup(std::shared_ptr<DeviceResources> deviceResources, PRIMITIVE_TOPOLOGY topology = PRIMITIVE_TOPOLOGY::TRIANGLELIST) noexcept :
		MeshGroupBase(deviceResources, topology) 
	{}
	inline MeshGroup(MeshGroup&& rhs) noexcept :
		MeshGroupBase(std::move(rhs)), // See this SO post above calling std::move(rhs) but then proceding to use the rhs object: https://stackoverflow.com/questions/22977230/move-constructors-in-inheritance-hierarchy
		m_vertices(std::move(rhs.m_vertices)),
		m_indices(std::move(rhs.m_indices))
	{}
	inline MeshGroup& operator=(MeshGroup&& rhs) noexcept
	{
		MeshGroupBase::operator=(std::move(rhs)); // See this SO post above calling std::move(rhs) but then proceding to use the rhs object: https://stackoverflow.com/questions/22977230/move-constructors-in-inheritance-hierarchy
		m_vertices = std::move(rhs.m_vertices);
		m_indices = std::move(rhs.m_indices);
		return *this;
	}
	inline MeshGroup(const MeshGroup& rhs) :
		MeshGroupBase(rhs),
		m_vertices(rhs.m_vertices),
		m_indices(rhs.m_indices)
	{
		FinalizePushBack(true);
	}
	inline MeshGroup& operator=(const MeshGroup& rhs)
	{
		MeshGroupBase::operator=(rhs);

		bool previouslyEmpty = m_vertices.size() == 0;

		m_vertices = rhs.m_vertices;
		m_indices = rhs.m_indices;

		FinalizePushBack(previouslyEmpty);
		return *this;
	}

	MeshGroup<T> CopySubset(unsigned int indexOfMeshToCopy);
	MeshGroup<T> CopySubset(std::span<unsigned int> indicesOfMeshesToCopy);

	inline virtual ~MeshGroup() noexcept override { CleanUp(); }

	// Return the index for the mesh in the group
	unsigned int PushBack(const Mesh<T>& mesh);
	unsigned int PushBack(Mesh<T>&& mesh);
	std::vector<unsigned int> PushBack(const std::vector<Mesh<T>>& meshes);
	std::vector<unsigned int> PushBack(std::vector<Mesh<T>>&& meshes);

private:
	void PushBackImpl(const Mesh<T>& mesh);
	void PushBackImpl(Mesh<T>&& mesh);
	void FinalizePushBack(bool previouslyEmpty);

	// System memory copies. 
	std::vector<T> m_vertices;
	std::vector<std::uint16_t> m_indices;
};

template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
unsigned int MeshGroup<T>::PushBack(const Mesh<T>& mesh)
{
	bool previouslyEmpty = m_vertices.size() == 0;

	PushBackImpl(mesh);
	FinalizePushBack(previouslyEmpty);

	return static_cast<unsigned int>(m_submeshes.size() - 1);
}

template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
unsigned int MeshGroup<T>::PushBack(Mesh<T>&& mesh)
{
	bool previouslyEmpty = m_submeshes.size() == 0;

	PushBackImpl(mesh);
	FinalizePushBack(previouslyEmpty);

	return static_cast<unsigned int>(m_submeshes.size() - 1);
}

template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
std::vector<unsigned int> MeshGroup<T>::PushBack(const std::vector<Mesh<T>>& meshes)
{
	std::vector<unsigned int> ret;
	ret.reserve(meshes.size());

	bool previouslyEmpty = m_vertices.size() == 0;

	// reserve space for all new vertices/indices
	size_t v_count = m_vertices.size();
	size_t i_count = m_indices.size();
	for (const auto& mesh : meshes)
	{
		v_count += mesh.m_vertices.size();
		i_count += mesh.m_indices.size();
	}
	m_vertices.reserve(v_count);
	m_indices.reserve(i_count);
	
	unsigned int iii = static_cast<unsigned int>(m_submeshes.size());
	for (const auto& mesh : meshes)
	{
		ret.push_back(iii++);
		PushBackImpl(mesh);
	}

	FinalizePushBack(previouslyEmpty);

	return ret;
}

template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
std::vector<unsigned int> MeshGroup<T>::PushBack(std::vector<Mesh<T>>&& meshes)
{
	std::vector<unsigned int> ret;
	ret.reserve(meshes.size());

	bool previouslyEmpty = m_vertices.size() == 0;

	// reserve space for all new vertices/indices
	size_t v_count = m_vertices.size();
	size_t i_count = m_indices.size();
	for (const auto& mesh : meshes)
	{
		v_count += mesh.m_vertices.size();
		i_count += mesh.m_vertices.size();
	}
	m_vertices.reserve(v_count);
	m_indices.reserve(i_count);

	unsigned int iii = static_cast<unsigned int>(m_submeshes.size());
	for (auto& mesh : meshes)
	{
		ret.push_back(iii++);
		PushBackImpl(std::move(mesh));
	}

	FinalizePushBack(previouslyEmpty);

	return ret;
}

template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
void MeshGroup<T>::PushBackImpl(const Mesh<T>& mesh)
{
	bool previouslyEmpty = m_vertices.size() == 0;

	// Create the new submesh structure for the mesh we are about to add
	m_submeshes.emplace_back(
		static_cast<UINT>(mesh.m_indices.size()), 
		static_cast<UINT>(m_indices.size()),
		static_cast<INT>(m_vertices.size()), 
		static_cast<UINT>(mesh.m_vertices.size()),
		mesh.m_bounds, mesh.m_sphere);

	// Reserve space and add new vertices/indices
	m_vertices.reserve(m_vertices.size() + mesh.m_vertices.size());
	m_vertices.insert(m_vertices.end(), mesh.m_vertices.begin(), mesh.m_vertices.end());
	m_indices.reserve(m_indices.size() + mesh.m_indices.size());
	m_indices.insert(m_indices.end(), mesh.m_indices.begin(), mesh.m_indices.end());
}

template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
void MeshGroup<T>::PushBackImpl(Mesh<T>&& mesh)
{
	bool previouslyEmpty = m_vertices.size() == 0;

	// Create the new submesh structure for the mesh we are about to add
	m_submeshes.emplace_back(
		static_cast<UINT>(mesh.m_indices.size()),
		static_cast<UINT>(m_indices.size()),
		static_cast<INT>(m_vertices.size()),
		static_cast<UINT>(mesh.m_vertices.size()),
		mesh.m_bounds, mesh.m_sphere);

	// Add new vertices/indices. Because we are being passed an r-value, if we are previously empty,
	// we can just move the data
	if (previouslyEmpty)
	{
		ASSERT(m_vertices.size() == 0, "Cannot be previously empty but have vertices");
		ASSERT(m_indices.size() == 0, "Cannot be previously empty but have indices");

		m_vertices = std::move(mesh.m_vertices);
		m_indices = std::move(mesh.m_indices);
	}
	else
	{
		m_vertices.reserve(m_vertices.size() + mesh.m_vertices.size());
		m_vertices.insert(m_vertices.end(), mesh.m_vertices.begin(), mesh.m_vertices.end());
		m_indices.reserve(m_indices.size() + mesh.m_indices.size());
		m_indices.insert(m_indices.end(), mesh.m_indices.begin(), mesh.m_indices.end());
	}
}

template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
void MeshGroup<T>::FinalizePushBack(bool previouslyEmpty)
{
	// Compute the vertex/index buffer view data
	m_vertexBufferView.StrideInBytes = sizeof(T);
	m_vertexBufferView.SizeInBytes = static_cast<UINT>(m_vertices.size()) * sizeof(T);
	m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	m_indexBufferView.SizeInBytes = static_cast<UINT>(m_indices.size()) * sizeof(std::uint16_t);

	// If the buffers were previously not empty, then we need to do a delayed delete of the resources
	if (!previouslyEmpty)
	{
		m_deviceResources->DelayedDelete(m_vertexBufferGPU);
		m_deviceResources->DelayedDelete(m_indexBufferGPU);
	}

	// Create all new buffers to hold the new data
	m_vertexBufferGPU = CreateDefaultBuffer(m_vertices.data(), m_vertexBufferView.SizeInBytes);
	m_indexBufferGPU = CreateDefaultBuffer(m_indices.data(), m_indexBufferView.SizeInBytes);

	// Get the buffer locations
	m_vertexBufferView.BufferLocation = m_vertexBufferGPU->GetGPUVirtualAddress();
	m_indexBufferView.BufferLocation = m_indexBufferGPU->GetGPUVirtualAddress();

#ifndef TOPO_DIST
	SetDebugName(m_name);
#endif
}

template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
MeshGroup<T> MeshGroup<T>::CopySubset(unsigned int indexOfMeshToCopy)
{
	ASSERT(indexOfMeshToCopy < m_submeshes.size(), "Index out of range");

	MeshGroup<T> dest(m_deviceResources);

	const MeshDescriptor& md = m_submeshes[indexOfMeshToCopy];

	dest.m_submeshes.emplace_back(
		md.IndexCount,
		0, 
		0,
		md.VertexCount,
		md.Bounds,
		md.Sphere
	);

	dest.m_vertices.reserve(md.VertexCount);
	std::copy(
		m_vertices.begin() + md.BaseVertexLocation, 
		m_vertices.begin() + md.BaseVertexLocation + md.VertexCount, 
		std::back_inserter(dest.m_vertices)
	);

	dest.m_indices.reserve(md.IndexCount);
	std::copy(
		m_indices.begin() + md.StartIndexLocation,
		m_indices.begin() + md.StartIndexLocation + md.IndexCount,
		std::back_inserter(dest.m_indices)
	);

	dest.FinalizePushBack(true); 
	return dest;
}

template<typename T>
requires HasMemberFunctionPositionThatReturnsXMFLOAT3<T>
MeshGroup<T> MeshGroup<T>::CopySubset(std::span<unsigned int> indicesOfMeshesToCopy)
{
	MeshGroup<T> dest(m_deviceResources);

	// Reserve space
	size_t indexCount = 0;
	size_t vertexCount = 0;
	for (unsigned int index : indicesOfMeshesToCopy)
	{
		ASSERT(index < m_submeshes.size(), "Index out of range"); 
		indexCount += m_submeshes[index].IndexCount;
		vertexCount += m_submeshes[index].VertexCount;
	}
	dest.m_indices.reserve(indexCount);
	dest.m_vertices.reserve(vertexCount);

	// Copy in new data
	int nextIndexStartPosition = 0;
	int nextVertexStartPosition = 0;
	for (unsigned int index : indicesOfMeshesToCopy)
	{
		const MeshDescriptor& md = m_submeshes[index];

		dest.m_submeshes.emplace_back(
			md.IndexCount,
			nextIndexStartPosition,
			nextVertexStartPosition,
			md.VertexCount,
			md.Bounds,
			md.Sphere
		);
		nextIndexStartPosition += md.IndexCount;
		nextVertexStartPosition += md.VertexCount;

		std::copy(
			m_vertices.begin() + md.BaseVertexLocation,
			m_vertices.begin() + md.BaseVertexLocation + md.VertexCount,
			std::back_inserter(dest.m_vertices)
		);

		std::copy(
			m_indices.begin() + md.StartIndexLocation,
			m_indices.begin() + md.StartIndexLocation + md.IndexCount,
			std::back_inserter(dest.m_indices)
		);
	}

	dest.FinalizePushBack(true);
	return dest;
}


// =====================================================================================================
// DynamicMeshBase 
//
// NOTE: For dynamic meshes, we make the simplification that the underlying MeshGroup will ONLY hold a single mesh.
//       The reason for this is that we have to change the vertex/index buffer view each frame as well as copy data
//		 from the CPU to GPU each frame which is all made easier by forcing the class to only manage a single Mesh
class DynamicMeshGroupBase : public MeshGroupBase
{
public:
	inline DynamicMeshGroupBase(std::shared_ptr<DeviceResources> deviceResources, PRIMITIVE_TOPOLOGY topology = PRIMITIVE_TOPOLOGY::TRIANGLELIST) :
		MeshGroupBase(deviceResources, topology) {}
	inline DynamicMeshGroupBase(DynamicMeshGroupBase&& rhs) noexcept : MeshGroupBase(std::move(rhs)) {}
	inline DynamicMeshGroupBase& operator=(DynamicMeshGroupBase&& rhs) noexcept
	{
		MeshGroupBase::operator=(std::move(rhs));
		return *this;
	}
	inline virtual ~DynamicMeshGroupBase() noexcept override {}
	inline void Update(int frameIndex) noexcept override
	{
		// For dynamic meshes, we keep gNumFrameResources copies of the vertex/index buffer in a single, continuous buffer
		// All we need to do every time Update() is called, is to update the vertex/index buffer views to point at the correct
		// starting location for the next buffer we want to use
		m_vertexBufferView.BufferLocation = m_vertexBufferGPU->GetGPUVirtualAddress() + static_cast<UINT64>(frameIndex) * m_vertexBufferView.SizeInBytes;
		m_indexBufferView.BufferLocation = m_indexBufferGPU->GetGPUVirtualAddress() + static_cast<UINT64>(frameIndex) * m_indexBufferView.SizeInBytes;
	}

protected:
	ND Microsoft::WRL::ComPtr<ID3D12Resource> CreateUploadBuffer(UINT64 totalBufferSize) const;

private:
	// There is too much state to worry about copying, so just delete copy operations until we find a good use case
	DynamicMeshGroupBase(const DynamicMeshGroupBase&) = delete;
	DynamicMeshGroupBase& operator=(const DynamicMeshGroupBase&) = delete;
};


// =====================================================================================================
// DynamicMeshGroup 
//
template<typename T>
class DynamicMeshGroup : public DynamicMeshGroupBase
{
public:
	// NOTE: For Dynamic meshes, we only allow there to be a single mesh - see Note above the DynamicMeshGroup class
	inline DynamicMeshGroup(std::shared_ptr<DeviceResources> deviceResources,
							std::vector<T>&& vertices,
							std::vector<std::uint16_t>&& indices,
							PRIMITIVE_TOPOLOGY topology = PRIMITIVE_TOPOLOGY::TRIANGLELIST) :
		DynamicMeshGroupBase(deviceResources, topology),
		m_vertices(std::move(vertices)),
		m_indices(std::move(indices))
	{
		ASSERT(m_vertices.size() > 0, "No vertices");
		ASSERT(m_indices.size() > 0, "No indices");

		// Create the submesh structure for the single mesh
		MeshDescriptor submesh;
		submesh.IndexCount = (UINT)m_indices.size();
		submesh.StartIndexLocation = 0;
		submesh.BaseVertexLocation = 0;
		m_submeshes.push_back(submesh);

		// Compute the vertex/index buffer view data
		m_vertexBufferView.StrideInBytes = sizeof(T);
		m_vertexBufferView.SizeInBytes = static_cast<UINT>(m_vertices.size()) * sizeof(T);
		m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
		m_indexBufferView.SizeInBytes = static_cast<UINT>(m_indices.size()) * sizeof(std::uint16_t);

		// Create the vertex and index buffers as UPLOAD buffers (so there will be gNumFrameResources copies of the vertex/index buffers)
		m_vertexBufferGPU = CreateUploadBuffer(m_vertexBufferView.SizeInBytes);
		m_indexBufferGPU = CreateUploadBuffer(m_indexBufferView.SizeInBytes);

		// Map the vertex and index buffers
		GFX_THROW_INFO(m_vertexBufferGPU->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedVertexData)));
		GFX_THROW_INFO(m_indexBufferGPU->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedIndexData)));

		// Copy the data into all slots of the upload buffers (We do all slots because creation of the dynamic buffer
		// may occur at any point, not necessarily just at program start up, so we can't just assume we are on frame index 0)
		for (unsigned int iii = 0; iii < g_numFrameResources; ++iii)
		{
			memcpy(&m_mappedVertexData[iii * m_vertexBufferView.SizeInBytes], m_vertices.data(), m_vertexBufferView.SizeInBytes);
			memcpy(&m_mappedIndexData[iii * m_indexBufferView.SizeInBytes], m_indices.data(), m_indexBufferView.SizeInBytes);
		}

		// Set the buffer locations as the start of the Upload buffers. This will later be changed each frame when Update() is called
		m_vertexBufferView.BufferLocation = m_vertexBufferGPU->GetGPUVirtualAddress();
		m_indexBufferView.BufferLocation = m_indexBufferGPU->GetGPUVirtualAddress();

#ifndef TOPO_DIST
		SetDebugName(m_name);
#endif
	}
	inline DynamicMeshGroup(DynamicMeshGroup&& rhs) noexcept :
		DynamicMeshGroupBase(std::move(rhs)),
		m_vertices(std::move(rhs.m_vertices)),
		m_indices(std::move(rhs.m_indices))
	{
		// Map the vertex and index buffers
		GFX_THROW_INFO(m_vertexBufferGPU->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedVertexData)));
		GFX_THROW_INFO(m_indexBufferGPU->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedIndexData)));
	}
	inline DynamicMeshGroup& operator=(DynamicMeshGroup&& rhs) noexcept
	{
		DynamicMeshGroupBase::operator=(std::move(rhs));
		m_vertices = std::move(rhs.m_vertices);
		m_indices = std::move(rhs.m_indices);

		// Map the vertex and index buffers
		GFX_THROW_INFO(m_vertexBufferGPU->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedVertexData)));
		GFX_THROW_INFO(m_indexBufferGPU->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedIndexData)));

		return *this;
	}
	inline virtual ~DynamicMeshGroup() noexcept override
	{
		if (m_vertexBufferGPU != nullptr)
			m_vertexBufferGPU->Unmap(0, nullptr);

		if (m_indexBufferGPU != nullptr)
			m_indexBufferGPU->Unmap(0, nullptr);

		CleanUp();
	}

	inline void CopyVertices(unsigned int frameIndex, std::vector<T>&& newVertices) noexcept
	{
		ASSERT(newVertices.size() == m_vertices.size(), "The new set of vertices must have the same total number as the original set");
		m_vertices = std::move(newVertices);

		UploadVertices(frameIndex);
	}
	inline void CopyIndices(unsigned int frameIndex, std::vector<std::uint16_t>&& newIndices) noexcept
	{
		ASSERT(newIndices.size() == m_indices.size(), "The new set of indices must have the same total number as the original set");
		m_indices = std::move(newIndices);

		UploadIndices(frameIndex);
	}

	ND constexpr std::vector<T>& GetVertices() noexcept { return m_vertices; }
	ND constexpr std::vector<std::uint16_t>& GetIndices() noexcept { return m_indices; }

private:
	// There is too much state to worry about copying, so just delete copy operations until we find a good use case
	DynamicMeshGroup(const DynamicMeshGroup&) = delete;
	DynamicMeshGroup& operator=(const DynamicMeshGroup&) = delete;

	inline void UploadVertices(unsigned int frameIndex) noexcept
	{
		ASSERT(frameIndex < g_numFrameResources, "Frame index is larger than expected");
		memcpy(&m_mappedVertexData[frameIndex * m_vertexBufferView.SizeInBytes], m_vertices.data(), m_vertexBufferView.SizeInBytes);
	}
	inline void UploadIndices(unsigned int frameIndex) noexcept
	{
		ASSERT(frameIndex < g_numFrameResources, "Frame index is larger than expected");
		memcpy(&m_mappedIndexData[frameIndex * m_indexBufferView.SizeInBytes], m_indices.data(), m_indexBufferView.SizeInBytes);
	}

	// System memory copies. 
	std::vector<T> m_vertices;
	std::vector<std::uint16_t> m_indices;

	BYTE* m_mappedVertexData = nullptr;
	BYTE* m_mappedIndexData = nullptr;
};
#endif
}