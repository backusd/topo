#pragma once
#include "topo/Core.h"
#include "topo/TopoException.h"
#include "Shader.h"
#include "topo/rendering/MeshGroup.h"
#include "Texture.h"

namespace topo
{
class AssetManager
{
	friend class Application;
	friend class Shader;
	friend class Texture;

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
	struct TextureBackingObjectAndRefCount
	{
		TextureBackingObjectAndRefCount(std::shared_ptr<DeviceResources> deviceResources, std::string_view textureFileName, unsigned int count = 1) :
			texture(deviceResources, textureFileName),
			refCount(count)
		{}

		TextureBackingObject texture;
		unsigned int refCount;
	};

public:
	// Shaders
	static Shader CheckoutShader(std::string_view shaderFileName) { return Get().CheckoutShaderImpl(shaderFileName); }
	static Shader CheckoutShader(std::string_view shaderFileName, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputs) { return Get().CheckoutShaderImpl(shaderFileName, inputs); }
	static Shader CheckoutShader(std::string_view shaderFileName, std::vector<D3D12_INPUT_ELEMENT_DESC>&& inputs) { return Get().CheckoutShaderImpl(shaderFileName, std::move(inputs)); }

	// Textures
	static inline Texture CheckoutTexture(std::shared_ptr<DeviceResources> deviceResources, std::string_view textureFileName) { return Get().CheckoutTextureImpl(deviceResources, textureFileName); }
	template<typename... T>
	static inline std::vector<Texture> CheckoutTextures(std::shared_ptr<DeviceResources> deviceResources, T... args) { return Get().CheckoutTexturesImpl(deviceResources, args...); }

private:
	static inline AssetManager& Get()
	{
		static AssetManager am{};
		return am;
	}

	AssetManager() noexcept {}
	AssetManager(const AssetManager&) = delete;
	AssetManager(AssetManager&&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;
	AssetManager& operator=(AssetManager&&) = delete;

	// Shaders
	static void ShaderIncrementCount(const std::string& key) noexcept { Get().ShaderIncrementCountImpl(key); }
	static void ShaderDecrementCount(const std::string& key) noexcept { Get().ShaderDecrementCountImpl(key); }
	void ShaderIncrementCountImpl(const std::string& key) noexcept;
	void ShaderDecrementCountImpl(const std::string& key) noexcept;
	Shader CheckoutShaderImpl(std::string_view shaderFileName);
	Shader CheckoutShaderImpl(std::string_view shaderFileName, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputs);
	Shader CheckoutShaderImpl(std::string_view shaderFileName, std::vector<D3D12_INPUT_ELEMENT_DESC>&& inputs);

	// Textures
	static void TextureIncrementCount(const std::string& key) noexcept { Get().TextureIncrementCountImpl(key); }
	static void TextureDecrementCount(const std::string& key) noexcept { Get().TextureDecrementCountImpl(key); }
	void TextureIncrementCountImpl(const std::string& key) noexcept;
	void TextureDecrementCountImpl(const std::string& key) noexcept;
	Texture CheckoutTextureImpl(std::shared_ptr<DeviceResources> deviceResources, std::string_view textureFileName);
	template<typename... T>
	inline std::vector<Texture> CheckoutTexturesImpl(std::shared_ptr<DeviceResources> deviceResources, T... args) 
	{ 
		ASSERT(sizeof...(args) >= 1, "args cannot be empty");
		// The following will be expanded to call CheckoutTextureImpl for each argument in the parameter pack in order
		return { CheckoutTextureImpl(deviceResources, args)... };
	}



	std::unordered_map<std::string, ShaderBackingObjectAndRefCount> m_shaders;
	std::unordered_map<std::string, TextureBackingObjectAndRefCount> m_textures;
};

}