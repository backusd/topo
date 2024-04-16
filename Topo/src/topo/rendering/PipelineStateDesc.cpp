#include "pch.h"
#include "PipelineStateDesc.h"


namespace topo
{
D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc::ConvertToDirectX12() const noexcept
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
	ZeroMemory(&desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	// Root Signature
	desc.pRootSignature = RootSignature->Get();

	// Shaders
	if (VertexShader.has_value())
		desc.VS = VertexShader.value().GetShaderByteCode();
	
	if (PixelShader.has_value())
		desc.PS = PixelShader.value().GetShaderByteCode();

	if (DomainShader.has_value())
		desc.DS = DomainShader.value().GetShaderByteCode();

	if (HullShader.has_value())
		desc.HS = HullShader.value().GetShaderByteCode();

	if (GeometryShader.has_value())
		desc.GS = GeometryShader.value().GetShaderByteCode();

	// StreamOutputDesc
	ASSERT(StreamOutputDesc == nullptr, "Haven't added support for StreamOutputDesc yet");

	// Blend State
	desc.BlendState.AlphaToCoverageEnable = BlendDesc.AlphaToCoverageEnable;
	desc.BlendState.IndependentBlendEnable = BlendDesc.IndependentBlendEnable;
	for (unsigned int iii = 0; iii < 8; ++iii)
	{
		desc.BlendState.RenderTarget[iii].BlendEnable = BlendDesc.RenderTarget[iii].BlendEnable;
		desc.BlendState.RenderTarget[iii].LogicOpEnable = BlendDesc.RenderTarget[iii].LogicOpEnable;
		desc.BlendState.RenderTarget[iii].SrcBlend = static_cast<D3D12_BLEND>(BlendDesc.RenderTarget[iii].SrcBlend);
		desc.BlendState.RenderTarget[iii].DestBlend = static_cast<D3D12_BLEND>(BlendDesc.RenderTarget[iii].DestBlend);
		desc.BlendState.RenderTarget[iii].BlendOp = static_cast<D3D12_BLEND_OP>(BlendDesc.RenderTarget[iii].BlendOp);
		desc.BlendState.RenderTarget[iii].SrcBlendAlpha = static_cast<D3D12_BLEND>(BlendDesc.RenderTarget[iii].SrcBlendAlpha); 
		desc.BlendState.RenderTarget[iii].DestBlendAlpha = static_cast<D3D12_BLEND>(BlendDesc.RenderTarget[iii].DestBlendAlpha);
		desc.BlendState.RenderTarget[iii].BlendOpAlpha = static_cast<D3D12_BLEND_OP>(BlendDesc.RenderTarget[iii].BlendOpAlpha);
		desc.BlendState.RenderTarget[iii].LogicOp = static_cast<D3D12_LOGIC_OP>(BlendDesc.RenderTarget[iii].LogicOp);
		desc.BlendState.RenderTarget[iii].RenderTargetWriteMask = BlendDesc.RenderTarget[iii].RenderTargetWriteMask;
	}

	// Sample Mask
	desc.SampleMask = SampleMask;

	// Rasterizer State
	desc.RasterizerState.FillMode = static_cast<D3D12_FILL_MODE>(RasterizerDesc.FillMode);
	desc.RasterizerState.CullMode = static_cast<D3D12_CULL_MODE>(RasterizerDesc.CullMode);
	desc.RasterizerState.FrontCounterClockwise = RasterizerDesc.FrontCounterClockwise;
	desc.RasterizerState.DepthBias = RasterizerDesc.DepthBias;
	desc.RasterizerState.DepthBiasClamp = RasterizerDesc.DepthBiasClamp;
	desc.RasterizerState.SlopeScaledDepthBias = RasterizerDesc.SlopeScaledDepthBias;
	desc.RasterizerState.DepthClipEnable = RasterizerDesc.DepthClipEnable;
	desc.RasterizerState.MultisampleEnable = RasterizerDesc.MultisampleEnable;
	desc.RasterizerState.AntialiasedLineEnable = RasterizerDesc.AntialiasedLineEnable;
	desc.RasterizerState.ForcedSampleCount = RasterizerDesc.ForcedSampleCount;
	desc.RasterizerState.ConservativeRaster = static_cast<D3D12_CONSERVATIVE_RASTERIZATION_MODE>(RasterizerDesc.ConservativeRaster);

	// Depth Stencil State
	desc.DepthStencilState.DepthEnable = DepthStencilDesc.DepthEnable;
	desc.DepthStencilState.DepthWriteMask = static_cast<D3D12_DEPTH_WRITE_MASK>(DepthStencilDesc.DepthWriteMask);
	desc.DepthStencilState.DepthFunc = static_cast<D3D12_COMPARISON_FUNC>(DepthStencilDesc.DepthFunc);
	desc.DepthStencilState.StencilEnable = DepthStencilDesc.StencilEnable;
	desc.DepthStencilState.StencilReadMask = DepthStencilDesc.StencilReadMask;
	desc.DepthStencilState.StencilWriteMask = DepthStencilDesc.StencilWriteMask;
	desc.DepthStencilState.FrontFace.StencilFailOp = static_cast<D3D12_STENCIL_OP>(DepthStencilDesc.FrontFace.StencilFailOp);
	desc.DepthStencilState.FrontFace.StencilDepthFailOp = static_cast<D3D12_STENCIL_OP>(DepthStencilDesc.FrontFace.StencilDepthFailOp);
	desc.DepthStencilState.FrontFace.StencilPassOp = static_cast<D3D12_STENCIL_OP>(DepthStencilDesc.FrontFace.StencilPassOp);
	desc.DepthStencilState.FrontFace.StencilFunc = static_cast<D3D12_COMPARISON_FUNC>(DepthStencilDesc.FrontFace.StencilFunc);
	desc.DepthStencilState.BackFace.StencilFailOp = static_cast<D3D12_STENCIL_OP>(DepthStencilDesc.BackFace.StencilFailOp);
	desc.DepthStencilState.BackFace.StencilDepthFailOp = static_cast<D3D12_STENCIL_OP>(DepthStencilDesc.BackFace.StencilDepthFailOp);
	desc.DepthStencilState.BackFace.StencilPassOp = static_cast<D3D12_STENCIL_OP>(DepthStencilDesc.BackFace.StencilPassOp);
	desc.DepthStencilState.BackFace.StencilFunc = static_cast<D3D12_COMPARISON_FUNC>(DepthStencilDesc.BackFace.StencilFunc);

	// Input Layout
	if (VertexShader.has_value())
		desc.InputLayout = VertexShader.value().GetInputLayoutDesc();

	// IBStripCutValue
	desc.IBStripCutValue = static_cast<D3D12_INDEX_BUFFER_STRIP_CUT_VALUE>(IBStripCutValue);

	// Primitive Topology Type
	//	desc.PrimitiveTopologyType = ...
	// NOTE: This is intentionally omitted. The topology type will be deduced later on, 
	// but before the pipeline state object is created

	// Render Targets
	desc.NumRenderTargets = NumRenderTargets;
	for (unsigned int iii = 0; iii < 8; ++iii)
		desc.RTVFormats[iii] = static_cast<DXGI_FORMAT>(RTVFormats[iii]);
	desc.DSVFormat = static_cast<DXGI_FORMAT>(DSVFormat);

	// Sample Desc
	desc.SampleDesc.Count = SampleDesc.Count;
	desc.SampleDesc.Quality = SampleDesc.Quality;

	// Node Mask
	desc.NodeMask = NodeMask;

	// CACHED_PIPELINE_STATE
	ASSERT(CachedPSO == nullptr, "Haven't added support for this yet");

	// Flags
	desc.Flags = static_cast<D3D12_PIPELINE_STATE_FLAGS>(Flags);

	return desc;
}
}