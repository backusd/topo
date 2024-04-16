#include "pch.h"
#include "Button.h"
#include "topo/rendering/AssetManager.h"

namespace topo
{
bool Button::m_isInitialized = false;

Button::Button() : Control() 
{
	if (!m_isInitialized)
		OneTimeInitialization();
}

void Button::OneTimeInitialization()
{
//	auto il = std::vector<D3D12_INPUT_ELEMENT_DESC>{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//	};
//	AssetManager::AddShader("Control-vs.cso", std::move(il)); 
//	AssetManager::AddShader("Control-ps.cso"); 



}

}