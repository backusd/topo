#pragma once
#include "topo/Core.h"


namespace topo
{
// Using Windows only macro here because there are so many references to D3D12
#ifdef TOPO_PLATFORM_WINDOWS

class InputLayout
{
public:
	constexpr InputLayout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputs) noexcept : 
		m_inputLayout(inputs) 
	{}
	constexpr InputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>&& inputs) noexcept : 
		m_inputLayout(std::move(inputs)) 
	{}
	constexpr InputLayout(const InputLayout& rhs) noexcept = default;
	constexpr InputLayout(InputLayout&& rhs) noexcept = default;
	constexpr InputLayout& operator=(const InputLayout& rhs) noexcept = default;
	constexpr InputLayout& operator=(InputLayout&& rhs) noexcept = default;
	~InputLayout() noexcept {}

	ND constexpr D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc() const noexcept { return { m_inputLayout.data(), (UINT)m_inputLayout.size() }; }

private:
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;


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