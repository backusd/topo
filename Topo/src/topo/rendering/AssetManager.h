#pragma once
#include "topo/Core.h"
#include "topo/TopoException.h"
#include "Shader.h"
#include "topo/rendering/MeshGroup.h"

namespace topo
{
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class TOPO_API AssetManager
{
	friend class Application;

private:
//	using AddMeshGroupReturnType = std::pair<std::_List_iterator<std::_List_val<std::_List_simple_types<std::pair<const std::string, std::unique_ptr<MeshGroupBase>>>>>, bool>;
	using AddShaderReturnType = std::pair<std::_List_iterator<std::_List_val<std::_List_simple_types<std::pair<const std::string, Shader>>>>, bool>;

public:
	template<typename T>
	static std::shared_ptr<MeshGroupBase> AddMeshGroup(std::string_view name, std::shared_ptr<DeviceResources> deviceResources) noexcept { return Get().AddMeshGroupImpl<T>(name, deviceResources); }
	
	static AddShaderReturnType AddShader(std::string_view shaderFileName) { return Get().AddShaderImpl(shaderFileName); }
	static AddShaderReturnType AddShader(std::string_view shaderFileName, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputs) { return Get().AddShaderImpl(shaderFileName, inputs); }
	static AddShaderReturnType AddShader(std::string_view shaderFileName, std::vector<D3D12_INPUT_ELEMENT_DESC>&& inputs) { return Get().AddShaderImpl(shaderFileName, std::move(inputs)); }
	static const Shader& GetShader(std::string_view shaderFileName) { return Get().GetShaderImpl(shaderFileName); }

private:
	static AssetManager& Get()
	{
		static AssetManager am{};
		return am;
	}
	static void Shutdown() { Get().ShutdownImpl(); }
	void ShutdownImpl()
	{
		m_meshGroups.clear();
		m_shaders.clear();
	}

	AssetManager() noexcept {}
	AssetManager(const AssetManager&) = delete;
	AssetManager(AssetManager&&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;
	AssetManager& operator=(AssetManager&&) = delete;

	inline AddShaderReturnType AddShaderImpl(std::string_view shaderFileName)
	{
		// emplace will try to construct the key string with the first parameter and
		// then construct a Shader with the second parameter. Because Shader has a 
		// constructor that takes a string_view, this just works. Also, if the key already
		// exists, it will not add anything return value will contain a bool that is false
		// to signify that nothing has been added
		return m_shaders.emplace(shaderFileName, shaderFileName);
	}
	inline AddShaderReturnType AddShaderImpl(std::string_view shaderFileName, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputs)
	{
		return m_shaders.emplace(std::piecewise_construct,
			std::forward_as_tuple(shaderFileName),
			std::forward_as_tuple(shaderFileName, inputs));
	}
	inline AddShaderReturnType AddShaderImpl(std::string_view shaderFileName, std::vector<D3D12_INPUT_ELEMENT_DESC>&& inputs)
	{
		return m_shaders.emplace(std::piecewise_construct,
			std::forward_as_tuple(shaderFileName),
			std::forward_as_tuple(shaderFileName, std::move(inputs)));
	}
	inline const Shader& GetShaderImpl(std::string_view shaderFileName) 
	{
		// NOTE: We can't just try to return 'm_shaders[shaderFileName]' because it requires
		// that Shader be default constructable, which it is not
		if (auto itr = m_shaders.find(std::string(shaderFileName)); itr != m_shaders.end())
			return itr->second;

		throw EXCEPTION(std::format("AssetManager: Failed to find shader with key '{0}'", shaderFileName));
	}


	template<typename T>
	std::shared_ptr<MeshGroupBase> AddMeshGroupImpl(std::string_view name, std::shared_ptr<DeviceResources> deviceResources) noexcept
	{
		std::shared_ptr<MeshGroupBase> meshGroup = std::make_shared<MeshGroup<T>>(deviceResources);
		//m_meshGroups[std::string(name)] = std::move(meshGroup);

		auto [itr, success] = m_meshGroups.emplace(std::piecewise_construct,
			std::forward_as_tuple(name), 
			std::forward_as_tuple(meshGroup));

		return meshGroup;
	}



	std::unordered_map<std::string, std::shared_ptr<MeshGroupBase>> m_meshGroups;
	std::unordered_map<std::string, Shader> m_shaders;
};
#pragma warning( pop )
}