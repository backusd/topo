#pragma once
#include "topo/DeviceResources.h"
#include "topo/Log.h"
#include "topo/utils/String.h"

namespace topo
{
#ifdef DIRECTX12

class Shader
{
public:
	inline Shader(std::string_view filename) : m_filename(filename)
	{
		ASSERT(m_filename.size() > 0, "Filename cannot be empty");
		ReadFileToBlob();
	}
	inline Shader(const Shader& rhs) :
		m_filename(rhs.m_filename),
		m_blob(nullptr)
#ifndef TOPO_DIST
		, m_name(rhs.m_name)
#endif
	{
		ReadFileToBlob();
	}
	inline Shader(Shader&& rhs) noexcept :
		m_filename(rhs.m_filename),
		m_blob(rhs.m_blob) // Just make a copy of the ComPtr to the underlying blob because the rhs object will die soon, so no need to worry about multiple objects managing the same blob
#ifndef TOPO_DIST
		, m_name(std::move(rhs.m_name))
#endif
	{}
	inline Shader& operator=(const Shader& rhs)
	{
		m_filename = rhs.m_filename;
		ReadFileToBlob();
#ifndef TOPO_DIST
		m_name = rhs.m_name;
#endif
		return *this;
	}
	inline Shader& operator=(Shader&& rhs) noexcept
	{
		m_filename = rhs.m_filename;
		m_blob = rhs.m_blob;
#ifndef TOPO_DIST
		m_name = std::move(rhs.m_name);
#endif
		return *this;
	}
	inline ~Shader() noexcept {}

	ND inline void* GetBufferPointer() const noexcept { return m_blob->GetBufferPointer(); }
	ND inline SIZE_T GetBufferSize() const noexcept { return m_blob->GetBufferSize(); }
	ND inline D3D12_SHADER_BYTECODE GetShaderByteCode() const noexcept { return { reinterpret_cast<BYTE*>(m_blob->GetBufferPointer()), m_blob->GetBufferSize() }; }

protected:
	inline void ReadFileToBlob()
	{
		std::wstring w_str = s2ws(m_filename);
		GFX_THROW_INFO(
			D3DReadFileToBlob(w_str.c_str(), m_blob.ReleaseAndGetAddressOf())
		);
	}

	std::string						 m_filename;
	Microsoft::WRL::ComPtr<ID3DBlob> m_blob;

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