#pragma once
#include "topo/Core.h"



namespace topo
{
#pragma warning( push )
#pragma warning( disable : 4251 ) // needs to have dll-interface to be used by clients of class
class TOPO_API AssetManager
{
public:
	static void AddMesh(int i) noexcept { Get().AddMeshImpl(i); }

private:
	static AssetManager& Get()
	{
		static AssetManager am{};
		return am;
	}

	AssetManager() noexcept {}
	AssetManager(const AssetManager&) = delete;
	AssetManager(AssetManager&&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;
	AssetManager& operator=(AssetManager&&) = delete;

	void AddMeshImpl(int i);

	std::vector<int> m_meshes;
};
#pragma warning( pop )
}