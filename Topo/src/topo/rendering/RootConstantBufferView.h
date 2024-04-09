#pragma once
#include "ConstantBuffer.h"
#include "topo/utils/Timer.h"

namespace topo
{
#ifdef DIRECTX12

class RootConstantBufferView
{
public:
	inline RootConstantBufferView(UINT rootParameterIndex, ConstantBufferBase* cb) noexcept :
		m_rootParameterIndex(rootParameterIndex),
		m_constantBuffer(cb)
	{
		ASSERT(m_constantBuffer != nullptr, "ConstantBuffer should not be nullptr");
	}
	RootConstantBufferView(const RootConstantBufferView&) noexcept = default;
	RootConstantBufferView(RootConstantBufferView&&) noexcept = default;
	RootConstantBufferView& operator=(const RootConstantBufferView&) noexcept = default;
	RootConstantBufferView& operator=(RootConstantBufferView&&) noexcept = default;

	ND constexpr UINT GetRootParameterIndex() const noexcept { return m_rootParameterIndex; }
	ND constexpr ConstantBufferBase* GetConstantBuffer() const noexcept { return m_constantBuffer; }

	// No Setters yet... wait until you have a use case

	std::function<void(const Timer&, int)> Update = [](const Timer&, int) {};

private:
	UINT				m_rootParameterIndex;
	ConstantBufferBase* m_constantBuffer;


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