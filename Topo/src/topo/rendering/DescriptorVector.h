#pragma once
#include "topo/Log.h"


namespace topo
{
#ifdef DIRECTX12

class DescriptorVector
{
public:
    DescriptorVector(Microsoft::WRL::ComPtr<ID3D12Device> d3dDevice, D3D12_DESCRIPTOR_HEAP_TYPE type, unsigned int initialCapacity = 8);
    inline DescriptorVector(DescriptorVector&& rhs) noexcept :
        m_device(rhs.m_device),
        m_count(rhs.m_count),
        m_capacity(rhs.m_capacity),
        m_handleIncrementSize(rhs.m_handleIncrementSize),
        m_descriptorHeapCopyable(rhs.m_descriptorHeapCopyable),
        m_descriptorHeapShaderVisible(rhs.m_descriptorHeapShaderVisible),
        m_cpuHeapStart(rhs.m_cpuHeapStart),
        m_gpuHeapStart(rhs.m_gpuHeapStart),
        m_type(rhs.m_type),
        m_releasedIndices(std::move(m_releasedIndices))
#ifndef TOPO_DIST
        , m_name(std::move(rhs.m_name))
#endif
    {
        LOG_WARN("{}", "DescriptorVector Move Constructor has been called, but I've never tested this function.");
    }
    inline DescriptorVector& operator=(DescriptorVector&& rhs) noexcept
    {
        LOG_WARN("{}", "DescriptorVector Move Assignment operator has been called, but I've never tested this function.");

        m_device = rhs.m_device;
        m_count = rhs.m_count;
        m_capacity = rhs.m_capacity;
        m_handleIncrementSize = rhs.m_handleIncrementSize;
        m_descriptorHeapCopyable = rhs.m_descriptorHeapCopyable;
        m_descriptorHeapShaderVisible = rhs.m_descriptorHeapShaderVisible;
        m_cpuHeapStart = rhs.m_cpuHeapStart;
        m_gpuHeapStart = rhs.m_gpuHeapStart;
        m_type = rhs.m_type;
        m_releasedIndices = std::move(rhs.m_releasedIndices);
#ifndef TOPO_DIST
        m_name = std::move(rhs.m_name);
#endif

        return *this;
    }
    inline ~DescriptorVector() noexcept {}

    ND constexpr unsigned int Count() const noexcept { return m_count; }
    ND constexpr unsigned int Capacity() const noexcept { return m_capacity; }
    ND constexpr D3D12_DESCRIPTOR_HEAP_TYPE Type() const noexcept { return m_type; }

    ND D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleAt(UINT index) const noexcept;
    ND D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandleAt(UINT index) const noexcept;
    ND inline ID3D12DescriptorHeap* GetRawHeapPointer() const noexcept { return m_descriptorHeapShaderVisible.Get(); }

    unsigned int EmplaceBackShaderResourceView(ID3D12Resource* pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc);
    unsigned int EmplaceBackConstantBufferView(const D3D12_CONSTANT_BUFFER_VIEW_DESC* desc);
    unsigned int EmplaceBackUnorderedAccessView(ID3D12Resource* pResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc);

    constexpr void IncrementCount() noexcept { ++m_count; }
    constexpr void DecrementCount() noexcept { --m_count; }

    void ReleaseAt(unsigned int index) noexcept;

private:
    // Delete copy constructor/assignment because these don't really make sense for the use case of DescriptorVector which,
    // is designed to manage a descriptor heap with a unique set of descriptors
    DescriptorVector(const DescriptorVector&) = delete;
    DescriptorVector& operator=(const DescriptorVector&) = delete;

    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCopyableHandleAt(UINT index) const noexcept;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCopyableHandleAt(UINT index) const noexcept;
    void DoubleTheCapacity();
    unsigned int GetNextIndexAndEnsureCapacity() noexcept;

    unsigned int m_count;
    unsigned int m_capacity;
    UINT m_handleIncrementSize;

    Microsoft::WRL::ComPtr<ID3D12Device> m_device;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorHeapCopyable;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_descriptorHeapShaderVisible;

    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuHeapStart;
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuHeapStart;

    D3D12_DESCRIPTOR_HEAP_TYPE m_type;

    std::vector<unsigned int> m_releasedIndices;



// In DIST builds, we don't name the object
#ifndef TOPO_DIST
public:
    void SetDebugName(std::string_view name) noexcept
    {
        m_name = name;
        std::string heap1Name = m_name + "-HeapCopyable";
        std::string heap2Name = m_name + "-HeapShaderVisible";
        m_descriptorHeapCopyable->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(heap1Name.size()), heap1Name.data());
        m_descriptorHeapShaderVisible->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(heap2Name.size()), heap2Name.data());
    }
    ND const std::string& GetDebugName() const noexcept
    {
        return m_name;
    }
private:
    std::string m_name;
#endif
};

#endif
}