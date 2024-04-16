#include "pch.h"
#include "Shader.h"
#include "AssetManager.h"

namespace topo
{
	Shader::Shader(const Shader& rhs) :
		m_key(rhs.m_key),
		m_shader(rhs.m_shader)
	{
		AssetManager::ShaderIncrementCount(m_key);
	}
	Shader::Shader(Shader&& rhs) noexcept :
		m_key(std::move(rhs.m_key)),
		m_shader(rhs.m_shader)
	{
		// When a shader has been moved from, m_key will no longer hold any data
		// Therefore, we don't need to increment/decrement the count because we
		// are simply 'moving' the reference to a different variable (and the old
		// one will no longer be valid)
	}
	Shader& Shader::operator=(const Shader& rhs)
	{
		m_key = rhs.m_key;
		m_shader = rhs.m_shader;
		AssetManager::ShaderIncrementCount(m_key);
		return *this;
	}
	Shader& Shader::operator=(Shader&& rhs) noexcept
	{
		// When a shader has been moved from, m_key will no longer hold any data
		// Therefore, we don't need to increment/decrement the count because we
		// are simply 'moving' the reference to a different variable (and the old
		// one will no longer be valid)
		m_key = std::move(rhs.m_key);
		m_shader = rhs.m_shader;
		return *this;
	}
	Shader::~Shader()
	{
		// When a shader has been moved from, m_key will no longer hold any data
		// Therefore, we should only decrement the count if m_key still holds a value
		if (!m_key.empty())
			AssetManager::ShaderDecrementCount(m_key);
	}
}