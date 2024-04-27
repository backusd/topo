#include "pch.h"
#include "UIRenderer.h"

using namespace DirectX;

namespace topo
{
void UIRenderer::SetDeviceResources(std::shared_ptr<DeviceResources> deviceResources)
{
	m_deviceResources = deviceResources;
	m_renderer.SetDeviceResources(deviceResources);

	InitializeRenderer();
}

void UIRenderer::InitializeRenderer()
{
	m_uiObjectConstantBuffer = std::make_unique<ConstantBufferMapped<UIObjectData>>(m_deviceResources);
	m_uiObjectConstantBuffer->Update = [this](const Timer& timer, int frameIndex)
		{
//				using namespace DirectX;
//
//				std::vector<UIObjectData> data(2); 
//
//				XMMATRIX world = XMMatrixScaling(200.0f, 200.0f, 1.0f) * XMMatrixTranslation(10.f, -10.0f, 0.0f);
//				XMStoreFloat4x4(&data[0].World, XMMatrixTranspose(world));
//
//				world = XMMatrixScaling(100.0f, 100.0f, 1.0f) * XMMatrixTranslation(210.f, -210.0f, 0.0f);
//				XMStoreFloat4x4(&data[1].World, XMMatrixTranspose(world));
//
//				m_uiObjectConstantBuffer->CopyData(frameIndex, data); 

			m_uiObjectConstantBuffer->CopyData(frameIndex, m_rectangleRenderItemTransforms);
		};

	std::vector<Vertex> squareVertices{
		{{  0.0f,  0.0f, 0.5f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }},
		{{  1.0f,  0.0f, 0.5f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f }},
		{{  1.0f, -1.0f, 0.5f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f }},
		{{  0.0f, -1.0f, 0.5f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }}
	};
	std::vector<std::uint16_t> squareIndices{ 0, 1, 3, 1, 2, 3 };
	Mesh<Vertex> mesh(std::move(squareVertices), std::move(squareIndices));

	m_meshGroup = std::make_unique<MeshGroup<Vertex>>(m_deviceResources, PRIMITIVE_TOPOLOGY::TRIANGLELIST);
	m_meshGroup->PushBack(std::move(mesh));

	m_uiPassConstantsBuffer = std::make_unique<ConstantBufferMapped<UIPassConstants>>(m_deviceResources);
	m_uiPassConstantsBuffer->Update = [this](const Timer& timer, int frameIndex)
		{
			using namespace DirectX;

			UIPassConstants pc{};
			XMStoreFloat4x4(&pc.View, XMMatrixTranspose(m_orthographicCamera.GetView()));
			XMStoreFloat4x4(&pc.InvView, XMMatrixTranspose(m_orthographicCamera.GetViewInverse()));
			XMStoreFloat4x4(&pc.Proj, XMMatrixTranspose(m_orthographicCamera.GetProj()));
			XMStoreFloat4x4(&pc.InvProj, XMMatrixTranspose(m_orthographicCamera.GetProjInverse()));
			XMStoreFloat4x4(&pc.ViewProj, XMMatrixTranspose(m_orthographicCamera.GetViewProj()));
			XMStoreFloat4x4(&pc.InvViewProj, XMMatrixTranspose(m_orthographicCamera.GetViewProjInverse()));
			pc.EyePosW = m_eyePosition;
			pc.RenderTargetSize = XMFLOAT2(this->GetWindowWidth(), this->GetWindowHeight());
			pc.InvRenderTargetSize = XMFLOAT2(1.0f / this->GetWindowWidth(), 1.0f / this->GetWindowHeight());
			pc.NearZ = 1.0f;
			pc.FarZ = 1000.0f;
			pc.TotalTime = timer.TotalTime();
			pc.DeltaTime = timer.DeltaTime();

			m_uiPassConstantsBuffer->CopyData(frameIndex, pc);
		};

	RenderPassSignature sig{
		ConstantBufferParameter{ 0 },
		ConstantBufferParameter{ 1 }
	};

	RenderPass& uiPass = m_renderer.EmplaceBackRenderPass(sig);
	SET_DEBUG_NAME(uiPass, "UI Render Pass");
	uiPass.BindConstantBuffer(1, m_uiPassConstantsBuffer.get());


	auto il = std::vector<D3D12_INPUT_ELEMENT_DESC>{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
	const Shader& vs = AssetManager::CheckoutShader("Control-vs.cso", std::move(il));
	const Shader& ps = AssetManager::CheckoutShader("Control-ps.cso");

	PipelineStateDesc psDesc{};
	psDesc.RootSignature = uiPass.GetRootSignature();
	psDesc.VertexShader = vs;
	psDesc.PixelShader = ps;
	psDesc.SampleMask = UINT_MAX; /// ??? Why?
	psDesc.NumRenderTargets = 1;
	psDesc.RTVFormats[0] = m_deviceResources->GetBackBufferFormat();
	psDesc.DSVFormat = m_deviceResources->GetDepthStencilFormat();

	RenderPassLayer& layer1 = uiPass.EmplaceBackRenderPassLayer(m_meshGroup.get(), psDesc);
	SET_DEBUG_NAME(layer1, "Opaque Layer");

	RenderItem& squareRI = layer1.EmplaceBackRenderItem(0, 0);
	SET_DEBUG_NAME(squareRI, "Rectangle RenderItem");

	squareRI.BindConstantBuffer(0, m_uiObjectConstantBuffer.get());
}

void UIRenderer::Update(const Timer& timer, int frameIndex) 
{ 
	RenderPass& pass1 = m_renderer.GetRenderPass(0);
	RenderPassLayer& opaqueLayer = pass1.GetRenderPassLayer(m_opaqueLayerIndex);
	RenderItem& rectangleRI = opaqueLayer.GetRenderItem(m_rectangleRenderItemIndex);

	unsigned int rectangleCount = m_rectangleRenderItemTransforms.size();
	if (rectangleCount == 0)
		opaqueLayer.SetActive(false);
	else
	{
		opaqueLayer.SetActive(true);
		rectangleRI.SetInstanceCount(rectangleCount);
	}



	m_renderer.Update(timer, frameIndex); 

	// 

	m_rectangleRenderItemTransforms.clear();
}

void UIRenderer::DrawRectangle(float left, float top, float right, float bottom)
{
	m_rectangleRenderItemTransforms.emplace_back();
	XMMATRIX world = XMMatrixScaling(right - left, bottom - top, 1.0f) * XMMatrixTranslation(left, -top, 0.0f);
	XMStoreFloat4x4(&m_rectangleRenderItemTransforms.back().World, XMMatrixTranspose(world));
}
void UIRenderer::DrawLine(float x1, float y1, float x2, float y2, float thickness)
{
	m_rectangleRenderItemTransforms.emplace_back();

	// 1. translate the original rectangle up 0.5 so it is centered on the y-axis
	// 2. scale the x direction to the length of the line and the y-direction to the thickness of the line
	// 3. rotate the line around the z-axis so it points in the direction it should
	// 4. translate the line to its final position
	XMMATRIX world = 
		XMMatrixTranslation(0.0f, 0.5f, 0.0f) *
		XMMatrixScaling(std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2)), thickness, 1.0f) *
		XMMatrixRotationZ(-std::atan2(y2 - y1, x2 - x1)) *
		XMMatrixTranslation(x1, -y1, 0.0f);
	XMStoreFloat4x4(&m_rectangleRenderItemTransforms.back().World, XMMatrixTranspose(world));
}
}