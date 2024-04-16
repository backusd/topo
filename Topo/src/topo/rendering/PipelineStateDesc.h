#pragma once
#include "RootSignature.h"
#include "Shader.h"
#include "Utility.h"


namespace topo
{
	struct StreamOutputDesc
	{
		// Placeholder class - NOT CURRENTLY SUPPORTED
	};
	struct RenderTargetBlendDesc
	{
		bool BlendEnable = false;
		bool LogicOpEnable = false;
		BLEND SrcBlend = BLEND::ONE;
		BLEND DestBlend = BLEND::ZERO;
		BLEND_OP BlendOp = BLEND_OP::ADD;
		BLEND SrcBlendAlpha = BLEND::ONE;
		BLEND DestBlendAlpha = BLEND::ZERO;
		BLEND_OP BlendOpAlpha = BLEND_OP::ADD;
		LOGIC_OP LogicOp = LOGIC_OP::NOOP;
		unsigned char RenderTargetWriteMask = static_cast<char>(COLOR_WRITE_ENABLE::ALL);
	};
	struct BlendDesc
	{
		bool AlphaToCoverageEnable = false;
		bool IndependentBlendEnable = false;
		RenderTargetBlendDesc RenderTarget[8] = {};
	};
	struct RasterizerDesc
	{
		FILL_MODE FillMode				= FILL_MODE::SOLID;
		CULL_MODE CullMode				= CULL_MODE::BACK;
		bool FrontCounterClockwise		= false;
		int DepthBias					= 0;
		float DepthBiasClamp			= 0.0f;
		float SlopeScaledDepthBias		= 0.0f;
		bool DepthClipEnable			= true;
		bool MultisampleEnable			= false;
		bool AntialiasedLineEnable		= false;
		unsigned int ForcedSampleCount	= 0;
		CONSERVATIVE_RASTERIZATION_MODE ConservativeRaster = CONSERVATIVE_RASTERIZATION_MODE::OFF;
	};
	struct DEPTH_STENCILOP_DESC
	{
		STENCIL_OP StencilFailOp		= STENCIL_OP::KEEP;
		STENCIL_OP StencilDepthFailOp	= STENCIL_OP::KEEP;
		STENCIL_OP StencilPassOp		= STENCIL_OP::KEEP;
		COMPARISON_FUNC StencilFunc		= COMPARISON_FUNC::ALWAYS;
	};
	struct DepthStencilDesc
	{
		bool DepthEnable				= true;
		DEPTH_WRITE_MASK DepthWriteMask	= DEPTH_WRITE_MASK::ALL;
		COMPARISON_FUNC DepthFunc		= COMPARISON_FUNC::LESS;
		bool StencilEnable				= false;
		unsigned char StencilReadMask	= 0xFF;
		unsigned char StencilWriteMask	= 0xFF;
		DEPTH_STENCILOP_DESC FrontFace	= {};
		DEPTH_STENCILOP_DESC BackFace	= {};
	};
	struct SAMPLE_DESC
	{
		unsigned int Count = 1;
		unsigned int Quality = 0;
	};
	struct CACHED_PIPELINE_STATE
	{
		// Placeholder class - NOT CURRENTLY SUPPORTED
	};
	struct PipelineStateDesc
	{
		const RootSignature* RootSignature = nullptr;
		std::optional<Shader> VertexShader = std::nullopt;
		std::optional<Shader> PixelShader = std::nullopt;
		std::optional<Shader> DomainShader = std::nullopt;
		std::optional<Shader> HullShader = std::nullopt;
		std::optional<Shader> GeometryShader = std::nullopt;
		StreamOutputDesc* StreamOutputDesc = nullptr; // NOT SUPPORTED
		const BlendDesc& BlendDesc = {};
		unsigned int SampleMask = 0;
		const RasterizerDesc& RasterizerDesc = {};
		const DepthStencilDesc& DepthStencilDesc = {};
		INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue = INDEX_BUFFER_STRIP_CUT_VALUE::VALUE_DISABLED;
		unsigned int NumRenderTargets = 0;
		FORMAT RTVFormats[8] = {};
		FORMAT DSVFormat = {};
		const SAMPLE_DESC& SampleDesc = {};
		unsigned int NodeMask = 0;
		CACHED_PIPELINE_STATE* CachedPSO = nullptr; // NOT SUPPORTED
		PIPELINE_STATE_FLAGS Flags = PIPELINE_STATE_FLAGS::NONE;

#ifdef DIRECTX12
		ND D3D12_GRAPHICS_PIPELINE_STATE_DESC ConvertToDirectX12() const noexcept;
#else
#error Only supporting DirectX12
#endif
	};
}