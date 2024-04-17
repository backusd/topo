#include "pch.h"
#include "Renderer.h"

namespace topo
{
#ifdef DIRECTX12

void Renderer::Update(const Timer& timer, int frameIndex)
{
	for (RenderPass& pass : m_renderPasses)
	{
		pass.Update(timer, frameIndex);

		for (RenderPassLayer& layer : pass.GetRenderPassLayers())
		{
			if (layer.IsActive())
				layer.Update(timer, frameIndex);
		}

		for (ComputeLayer& layer : pass.GetComputeLayers())
		{
			if (layer.IsActive())
				layer.Update(timer, frameIndex);
		}
	}
}

void Renderer::Render(int frameIndex)
{
	ASSERT(m_renderPasses.size() > 0, "No render passes");

	auto commandList = m_deviceResources->GetCommandList();

	GFX_THROW_INFO_ONLY(commandList->RSSetViewports(1, &m_viewport));
	GFX_THROW_INFO_ONLY(commandList->RSSetScissorRects(1, &m_scissorRect));

	for (RenderPass& pass : m_renderPasses)
	{
		ASSERT(pass.GetRenderPassLayers().size() > 0 || pass.GetComputeLayers().size() > 0, "Pass has no render layers nor compute layers. Must have at least 1 type of layer to be valid.");

		// Before attempting to perform any rendering, first perform all compute operations
		for (const ComputeLayer& layer : pass.GetComputeLayers())
		{
			if (!layer.IsActive())
				continue;

			RunComputeLayer(layer, nullptr, frameIndex); // NOTE: Pass nullptr for the timer, because we do not have access to the timer during the Rendering phase
		}

		// Pre-Work method - possibly for transitioning resources or anything necessary
		if (!pass.PreWork(pass, commandList))
			continue;

		// Bind the RenderPass data. This will set a single root signature for the render pass
		// and bind all RootConstantBufferViews owned by the pass
		pass.Bind(commandList, frameIndex);

		// Render the render layers for the pass
		for (const RenderPassLayer& layer : pass.GetRenderPassLayers())
		{
			if (!layer.IsActive())
				continue;

			ASSERT(layer.GetRenderItems().size() > 0, "Layer has no render items");
			ASSERT(layer.GetPSO() != nullptr, "Layer has no pipeline state");
			ASSERT(layer.GetMeshGroup() != nullptr, "Layer has no mesh group");

			// Apply stencil ref (if necessary)
			std::optional<unsigned int> stencilRef = layer.GetStencilRef();
			if (stencilRef.has_value())
			{
				GFX_THROW_INFO_ONLY(commandList->OMSetStencilRef(stencilRef.value()));
			}

			// PSO / Pre-Work / MeshGroup / Primitive Topology
			GFX_THROW_INFO_ONLY(commandList->SetPipelineState(layer.GetPSO()));

			if (!layer.PreWork(layer, commandList))		// Pre-Work method (example usage: setting stencil value)
				continue;

			MeshGroupBase* meshGroup = layer.GetMeshGroup();

			// Will set vertex buffers/index buffers/topology
			meshGroup->Bind(commandList);

			for (const RenderItem& item : layer.GetRenderItems())
			{
				if (!item.IsActive())
					continue;

				// Tables and CBV's ARE allowed to be empty
				for (const RootDescriptorTable& table : item.GetRootDescriptorTables())
				{
					GFX_THROW_INFO_ONLY(
						commandList->SetGraphicsRootDescriptorTable(table.GetRootParameterIndex(), table.GetDescriptorHandle())
					);
				}

				for (const RootConstantBufferView& cbv : item.GetRootConstantBufferViews())
				{
					GFX_THROW_INFO_ONLY(
						commandList->SetGraphicsRootConstantBufferView(cbv.GetRootParameterIndex(), cbv.GetConstantBuffer()->GetGPUVirtualAddress(frameIndex))
					);
				}

				const MeshDescriptor& mesh = meshGroup->GetSubmesh(item.GetSubmeshIndex());
				GFX_THROW_INFO_ONLY(
					commandList->DrawIndexedInstanced(mesh.IndexCount, item.GetInstanceCount(), mesh.StartIndexLocation, mesh.BaseVertexLocation, 0)
				);
			}
		}

		pass.PostWork(pass, commandList);
	}
}

void Renderer::RunComputeLayer(const ComputeLayer& layer, const Timer* timer, int frameIndex)
{
	auto commandList = m_deviceResources->GetCommandList();

	ASSERT(layer.GetComputeItems().size() > 0, "Compute layer has no compute items");

	// Pre-Work method - possibly for transitioning resources
	//		If it returns false, that means we should quit early and not call Dispatch for this RenderLayer
	if (!layer.PreWork(layer, commandList, timer, frameIndex))
		return;

	// Root Signature / PSO
	GFX_THROW_INFO_ONLY(commandList->SetComputeRootSignature(layer.GetRootSignature()->Get()));
	GFX_THROW_INFO_ONLY(commandList->SetPipelineState(layer.GetPSO()));

	// Iterate over each compute item and call dispatch to submit compute work to the GPU
	for (const ComputeItem& item : layer.GetComputeItems())
	{
		if (!item.IsActive())
			continue;

		// Tables and CBV's ARE allowed to be empty
		for (const RootDescriptorTable& table : item.GetRootDescriptorTables())
		{
			GFX_THROW_INFO_ONLY(
				commandList->SetComputeRootDescriptorTable(table.GetRootParameterIndex(), table.GetDescriptorHandle())
			);
		}
		for (const RootConstantBufferView& cbv : item.GetRootConstantBufferViews())
		{
			GFX_THROW_INFO_ONLY(
				commandList->SetComputeRootConstantBufferView(cbv.GetRootParameterIndex(), cbv.GetConstantBuffer()->GetGPUVirtualAddress(frameIndex))
			);
		}

		GFX_THROW_INFO_ONLY(commandList->Dispatch(item.GetThreadGroupCountX(), item.GetThreadGroupCountY(), item.GetThreadGroupCountZ()));
	}

	// Post-Work method - possibly for transitioning resources
	layer.PostWork(layer, commandList, timer, frameIndex);
}

#endif
}