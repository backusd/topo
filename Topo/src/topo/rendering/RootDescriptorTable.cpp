#include "pch.h"
#include "RootDescriptorTable.h"


namespace topo
{
void RootDescriptorTable::CreateSRV(const Texture& texture)
{
	// Create the SRV descriptor for the texture
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = texture.GetFormat();
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	m_srvIndices.push_back(m_descriptorVector->EmplaceBackShaderResourceView(texture.GetResource(), &srvDesc));
}
void RootDescriptorTable::CreateSRV(std::span<Texture> textures)
{
	ASSERT(textures.size() > 0, "No textures");
	ASSERT(textures[0].HasData(), "Invalid texture");

	// Create the SRV descriptor for the texture
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textures[0].GetFormat();
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	std::vector<std::pair<ID3D12Resource*, D3D12_SHADER_RESOURCE_VIEW_DESC>> data(textures.size());
	for (unsigned int iii = 0; iii < textures.size(); ++iii)
	{
		data[iii].first = textures[iii].GetResource();
		data[iii].second = srvDesc;
	}

	std::vector<unsigned int> indices = m_descriptorVector->EmplaceBackShaderResourceViews(data);
	m_srvIndices.append_range(indices);
}

}