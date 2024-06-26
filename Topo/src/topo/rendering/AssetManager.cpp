#include "pch.h"
#include "AssetManager.h"
#include "topo/TopoException.h"


namespace topo
{
Shader AssetManager::CheckoutShaderImpl(std::string_view shaderFileName)
{
	Shader shader;

	std::string key(shaderFileName);
	auto entry = m_shaders.find(key);

	// If the key does not exist, then emplace a new ShaderAndRefCount value (which will initialize a ref count of 1)
	if (entry == m_shaders.end())
	{
		// Doing a piecewise_construct means we can manually forward the necessary parameters to the key/value constructors
		auto itr = m_shaders.emplace(std::piecewise_construct,
			std::forward_as_tuple(key),
			std::forward_as_tuple(shaderFileName));

		// emplace returns a pair, and the second value is a bool for whether or not the emplace succeeded
		if (!(itr.second))
		{
			throw TopoException(std::format("Failed to add shader to AssetManager: {0}", key));
		}

		entry = itr.first;
	}
	else
	{
		entry->second.refCount++;
	}

	shader.m_shader = &(entry->second.shader);
	SET_DEBUG_NAME_PTR(shader.m_shader, key);
	return shader;
}
Shader AssetManager::CheckoutShaderImpl(std::string_view shaderFileName, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputs)
{
	Shader shader;

	std::string key(shaderFileName);
	auto entry = m_shaders.find(key);

	// If the key does not exist, then emplace a new ShaderAndRefCount value (which will initialize a ref count of 1)
	if (entry == m_shaders.end())
	{
		// Doing a piecewise_construct means we can manually forward the necessary parameters to the key/value constructors
		auto itr = m_shaders.emplace(std::piecewise_construct,
			std::forward_as_tuple(key),
			std::forward_as_tuple(shaderFileName, inputs));

		// emplace returns a pair, and the second value is a bool for whether or not the emplace succeeded
		if (!(itr.second))
		{
			throw TopoException(std::format("Failed to add shader to AssetManager: {0}", key));
		}

		entry = itr.first;
	}
	else
	{
		entry->second.refCount++;
	}

	shader.m_shader = &(entry->second.shader);
	SET_DEBUG_NAME_PTR(shader.m_shader, key);
	return shader;
}
Shader AssetManager::CheckoutShaderImpl(std::string_view shaderFileName, std::vector<D3D12_INPUT_ELEMENT_DESC>&& inputs)
{
	Shader shader;

	std::string key(shaderFileName);
	auto entry = m_shaders.find(key);

	// If the key does not exist, then emplace a new ShaderAndRefCount value (which will initialize a ref count of 1)
	if (entry == m_shaders.end())
	{
		// Doing a piecewise_construct means we can manually forward the necessary parameters to the key/value constructors
		auto itr = m_shaders.emplace(std::piecewise_construct,
			std::forward_as_tuple(key),
			std::forward_as_tuple(shaderFileName, std::move(inputs)));

		// emplace returns a pair, and the second value is a bool for whether or not the emplace succeeded
		if (!(itr.second))
		{
			throw TopoException(std::format("Failed to add shader to AssetManager: {0}", key));
		}

		entry = itr.first;
	}
	else
	{
		entry->second.refCount++;
	}

	shader.m_shader = &(entry->second.shader);
	SET_DEBUG_NAME_PTR(shader.m_shader, key);
	return shader;
}
void AssetManager::ShaderIncrementCountImpl(const std::string& key) noexcept
{
	ASSERT(key.size() > 0, "Should not have empty keys");
	auto entry = m_shaders.find(key);
	if (entry == m_shaders.end())
	{
		LOG_ERROR("AssetManager: Cannot increment ref count - failed to find shader '{0}'", key);
		return;
	}
	entry->second.refCount++;
}
void AssetManager::ShaderDecrementCountImpl(const std::string& key) noexcept
{
	ASSERT(key.size() > 0, "Should not have empty keys");
	auto entry = m_shaders.find(key);
	if (entry == m_shaders.end())
	{
		LOG_ERROR("AssetManager: Cannot decrement ref count - failed to find shader '{0}'", key);
		return;
	}
	entry->second.refCount--;

	if (entry->second.refCount == 0)
		m_shaders.erase(entry);
}


// Textures
Texture AssetManager::CheckoutTextureImpl(std::shared_ptr<DeviceResources> deviceResources, std::string_view textureFileName)
{
	Texture texture;

	std::string key = std::format("{0} {1}", deviceResources->Name(), textureFileName);
	auto entry = m_textures.find(key);

	// If the key does not exist, then emplace a new ShaderAndRefCount value (which will initialize a ref count of 1)
	if (entry == m_textures.end())
	{
		// Doing a piecewise_construct means we can manually forward the necessary parameters to the key/value constructors
		auto itr = m_textures.emplace(std::piecewise_construct,
			std::forward_as_tuple(key),
			std::forward_as_tuple(deviceResources, textureFileName));

		// emplace returns a pair, and the second value is a bool for whether or not the emplace succeeded
		if (!(itr.second))
		{
			throw TopoException(std::format("Failed to add shader to AssetManager: {0}", key));
		}

		entry = itr.first;
	}
	else
	{
		entry->second.refCount++;
	}

	texture.m_texture = &(entry->second.texture);
	SET_DEBUG_NAME_PTR(texture.m_texture, key);
	return texture;
}

void AssetManager::TextureIncrementCountImpl(const std::string& key) noexcept
{
	ASSERT(key.size() > 0, "Should not have empty keys"); 
	auto entry = m_textures.find(key);
	if (entry == m_textures.end())
	{
		LOG_ERROR("AssetManager: Cannot increment ref count - failed to find texture '{0}'", key);
		return;
	}
	entry->second.refCount++; 
}
void AssetManager::TextureDecrementCountImpl(const std::string& key) noexcept
{
	ASSERT(key.size() > 0, "Should not have empty keys"); 
	auto entry = m_textures.find(key);
	if (entry == m_textures.end())
	{
		LOG_ERROR("AssetManager: Cannot decrement ref count - failed to find texture '{0}'", key);
		return;
	}
	entry->second.refCount--; 

	if (entry->second.refCount == 0) 
		m_textures.erase(entry); 
}
}