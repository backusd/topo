#include "pch.h"
#include "Shader.h"
#include "AssetManager.h"

namespace topo
{
Shader::Shader(const Shader& rhs) :
	m_shader(rhs.m_shader)
{
	if (m_shader != nullptr)
		AssetManager::ShaderIncrementCount(m_shader->Filename());
}
Shader::Shader(Shader&& rhs) noexcept :
	m_shader(rhs.m_shader)
{
	// Set rhs shader to nullptr on the moved-from object so that it won't decrement the ref count
	rhs.m_shader = nullptr;
}
Shader& Shader::operator=(const Shader& rhs)
{
	m_shader = rhs.m_shader;

	if (m_shader != nullptr)
		AssetManager::ShaderIncrementCount(m_shader->Filename());

	return *this;
}
Shader& Shader::operator=(Shader&& rhs) noexcept
{
	m_shader = rhs.m_shader;

	// Set rhs shader to nullptr on the moved-from object so that it won't decrement the ref count
	rhs.m_shader = nullptr;

	return *this;
}
Shader::~Shader()
{
	if (m_shader != nullptr)
		AssetManager::ShaderDecrementCount(m_shader->Filename());
}
}