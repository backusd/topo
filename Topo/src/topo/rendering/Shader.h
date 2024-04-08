#pragma once
#include "topo/DeviceResources.h"
#include "topo/Log.h"


namespace topo
{
#ifdef TOPO_PLATFORM_WINDOWS

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
	{
		ReadFileToBlob();
	}
	inline Shader(Shader&& rhs) noexcept :
		m_filename(rhs.m_filename),
		m_blob(rhs.m_blob) // Just make a copy of the ComPtr to the underlying blob because the rhs object will die soon, so no need to worry about multiple objects managing the same blob
	{}
	inline Shader& operator=(const Shader& rhs)
	{
		m_filename = rhs.m_filename;
		ReadFileToBlob();
		return *this;
	}
	inline Shader& operator=(Shader&& rhs) noexcept
	{
		m_filename = rhs.m_filename;
		m_blob = rhs.m_blob;
		return *this;
	}
	inline ~Shader() noexcept {}

	ND inline void* GetBufferPointer() const noexcept { return m_blob->GetBufferPointer(); }
	ND inline SIZE_T GetBufferSize() const noexcept { return m_blob->GetBufferSize(); }
	ND inline D3D12_SHADER_BYTECODE GetShaderByteCode() const noexcept { return { reinterpret_cast<BYTE*>(m_blob->GetBufferPointer()), m_blob->GetBufferSize() }; }

protected:
	inline void ReadFileToBlob()
	{
		std::wstring w_str(m_filename.begin(), m_filename.end());
		GFX_THROW_INFO(
			D3DReadFileToBlob(w_str.c_str(), m_blob.ReleaseAndGetAddressOf())
		);
	}

	std::string						 m_filename;
	Microsoft::WRL::ComPtr<ID3DBlob> m_blob;
};

#endif
}