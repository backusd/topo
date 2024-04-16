#pragma once
#include "topo/Core.h"
#include "topo/TopoException.h"
#include "Shader.h"
#include "topo/rendering/MeshGroup.h"

namespace topo
{

class AssetManager
{
	friend class Application;
	friend class Shader;

	struct ShaderBackingObjectAndRefCount
	{
		ShaderBackingObjectAndRefCount(std::string_view shaderFileName, unsigned int count = 1) :
			shader(shaderFileName),
			refCount(count)
		{}
		ShaderBackingObjectAndRefCount(std::string_view shaderFileName, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputs, unsigned int count = 1) :
			shader(shaderFileName, inputs),
			refCount(count)
		{}
		ShaderBackingObjectAndRefCount(std::string_view shaderFileName, std::vector<D3D12_INPUT_ELEMENT_DESC>&& inputs, unsigned int count = 1) :
			shader(shaderFileName, std::move(inputs)),
			refCount(count)
		{}

		ShaderBackingObject shader;
		unsigned int refCount;
	};

public:
	// Shaders
	static Shader CheckoutShader(std::string_view shaderFileName) { return Get().CheckoutShaderImpl(shaderFileName); }
	static Shader CheckoutShader(std::string_view shaderFileName, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputs) { return Get().CheckoutShaderImpl(shaderFileName, inputs); }
	static Shader CheckoutShader(std::string_view shaderFileName, std::vector<D3D12_INPUT_ELEMENT_DESC>&& inputs) { return Get().CheckoutShaderImpl(shaderFileName, std::move(inputs)); }

	// Textures


private:
	static AssetManager& Get()
	{
		static AssetManager am{};
		return am;
	}
	static void Shutdown() { Get().ShutdownImpl(); }
	void ShutdownImpl()
	{
		m_shaders.clear();
	}

	AssetManager() noexcept {}
	AssetManager(const AssetManager&) = delete;
	AssetManager(AssetManager&&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;
	AssetManager& operator=(AssetManager&&) = delete;

	static void ShaderIncrementCount(const std::string& key) noexcept { Get().ShaderIncrementCountImpl(key); }
	static void ShaderDecrementCount(const std::string& key) noexcept { Get().ShaderDecrementCountImpl(key); }
	void ShaderIncrementCountImpl(const std::string& key) noexcept;
	void ShaderDecrementCountImpl(const std::string& key) noexcept;

	Shader CheckoutShaderImpl(std::string_view shaderFileName);
	Shader CheckoutShaderImpl(std::string_view shaderFileName, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputs);
	Shader CheckoutShaderImpl(std::string_view shaderFileName, std::vector<D3D12_INPUT_ELEMENT_DESC>&& inputs);


	std::unordered_map<std::string, ShaderBackingObjectAndRefCount> m_shaders;
};

}