#pragma once
#include "topo/Core.h"
#include "topo/DeviceResources.h"
#include "topo/Log.h"
#include "topo/utils/String.h"
#include "InputLayout.h"

namespace topo
{
#ifdef DIRECTX12

class ShaderBackingObject
{
public:
	inline ShaderBackingObject(std::string_view filename) :
		m_filename(filename),
		m_inputLayout(nullptr)
	{
		ASSERT(m_filename.size() > 0, "Filename cannot be empty");
		ASSERT(!ends_with(m_filename, "-vs.cso"), "Vertex shaders must be constructed with InputLayout data");
		ReadFileToBlob();
	}
	inline ShaderBackingObject(std::string_view filename, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputs) :
		m_filename(filename),
		m_inputLayout(std::make_unique<InputLayout>(inputs))
	{
		ASSERT(m_filename.size() > 0, "Filename cannot be empty");
		ASSERT(ends_with(m_filename, "-vs.cso"), "Only Vertex shaders need to be constructed with InputLayout data");
		ReadFileToBlob();
		SET_DEBUG_NAME_PTR(m_inputLayout, std::format("Input Layout ({0})", m_filename));
	}
	inline ShaderBackingObject(std::string_view filename, std::vector<D3D12_INPUT_ELEMENT_DESC>&& inputs) :
		m_filename(filename),
		m_inputLayout(std::make_unique<InputLayout>(std::move(inputs)))
	{
		ASSERT(m_filename.size() > 0, "Filename cannot be empty");
		ASSERT(ends_with(m_filename, "-vs.cso"), "Only Vertex shaders need to be constructed with InputLayout data");
		ReadFileToBlob();
		SET_DEBUG_NAME_PTR(m_inputLayout, std::format("Input Layout ({0})", m_filename));
	}


	inline ShaderBackingObject(ShaderBackingObject&& rhs) noexcept :
		m_filename(rhs.m_filename),
		m_blob(rhs.m_blob), // Just make a copy of the ComPtr to the underlying blob because the rhs object will die soon, so no need to worry about multiple objects managing the same blob
		m_inputLayout(std::move(rhs.m_inputLayout))
#ifndef TOPO_DIST
		, m_name(std::move(rhs.m_name))
#endif
	{}
	inline ShaderBackingObject& operator=(ShaderBackingObject&& rhs) noexcept
	{
		m_filename = rhs.m_filename;
		m_blob = rhs.m_blob;
		m_inputLayout = std::move(rhs.m_inputLayout);
#ifndef TOPO_DIST
		m_name = std::move(rhs.m_name);
#endif
		return *this;
	}

	ND inline void* GetBufferPointer() const noexcept { return m_blob->GetBufferPointer(); }
	ND inline SIZE_T GetBufferSize() const noexcept { return m_blob->GetBufferSize(); }
	ND inline D3D12_SHADER_BYTECODE GetShaderByteCode() const noexcept { return { reinterpret_cast<BYTE*>(m_blob->GetBufferPointer()), m_blob->GetBufferSize() }; }
	ND constexpr D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc() const noexcept
	{
		ASSERT(m_inputLayout != nullptr, "No InputLayout. Should only be calling Shader->GetInputLayoutDesc() for vertex shaders");
		return m_inputLayout->GetInputLayoutDesc();
	}
	ND inline const std::string& Filename() const noexcept { return m_filename; }


protected:
	// We hold a unique_ptr to the InputLayout so copying is non-trivial
	// Also, the AssetManager should own all shaders and we should only need to move
	// them, never copy
	ShaderBackingObject(const ShaderBackingObject&) = delete;
	ShaderBackingObject& operator=(const ShaderBackingObject&) = delete;

	inline void ReadFileToBlob()
	{
		// Assume that all *.cso files are in a 'cso' directory within the current working directory
		std::wstring w_str = L"cso/" + s2ws(m_filename);
		GFX_THROW_INFO(
			D3DReadFileToBlob(w_str.c_str(), m_blob.ReleaseAndGetAddressOf())
		);
	}

	std::string						 m_filename;
	Microsoft::WRL::ComPtr<ID3DBlob> m_blob;
	std::unique_ptr<InputLayout>	 m_inputLayout; // required for vertex shaders




	// In DIST builds, we don't name the object
#ifndef TOPO_DIST
public:
	void SetDebugName(std::string_view name) noexcept { m_name = name; }
	ND const std::string& GetDebugName() const noexcept { return m_name; }
private:
	std::string m_name;
#endif
};




class AssetManager;

class Shader
{
	friend class AssetManager;

public:
	inline Shader() noexcept :
		m_shader(nullptr)
	{}
	inline Shader(ShaderBackingObject* shader) :
		m_shader(shader)
	{}
	Shader(const Shader& rhs);
	Shader(Shader&& rhs) noexcept;
	Shader& operator=(const Shader& rhs);
	Shader& operator=(Shader&& rhs) noexcept;
	~Shader();

	ND inline D3D12_SHADER_BYTECODE GetShaderByteCode() const noexcept { return m_shader->GetShaderByteCode(); }
	ND constexpr D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc() const noexcept { return m_shader->GetInputLayoutDesc(); }

private:
	ShaderBackingObject* m_shader;
};



#endif
}