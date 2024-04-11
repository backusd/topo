#include "pch.h"
#include "AssetManager.h"


namespace topo
{
void AssetManager::AddMeshImpl(int i)
{
	m_meshes.push_back(i);
}
}