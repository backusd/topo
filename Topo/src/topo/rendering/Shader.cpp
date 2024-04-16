#include "pch.h"
#include "Shader.h"
#include "AssetManager.h"

namespace topo
{
Shader::Shader(const Shader& rhs) :
	m_shader(rhs.m_shader)
{
	AssetManager::ShaderIncrementCount(m_shader->Filename());
}
Shader::Shader(Shader&& rhs) noexcept :
	m_shader(rhs.m_shader)
{
	AssetManager::ShaderIncrementCount(m_shader->Filename());
}
Shader& Shader::operator=(const Shader& rhs)
{
	m_shader = rhs.m_shader;
	AssetManager::ShaderIncrementCount(m_shader->Filename());
	return *this;
}
Shader& Shader::operator=(Shader&& rhs) noexcept
{
	m_shader = rhs.m_shader;
	AssetManager::ShaderIncrementCount(m_shader->Filename());
	return *this;
}
Shader::~Shader()
{
	AssetManager::ShaderDecrementCount(m_shader->Filename());
}
}