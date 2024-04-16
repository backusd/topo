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

	shader.m_key = key;
	shader.m_shader = &(entry->second.shader);
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

	shader.m_key = key;
	shader.m_shader = &(entry->second.shader);
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

	shader.m_key = key;
	shader.m_shader = &(entry->second.shader);
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
}