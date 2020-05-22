#include "GraphicsRenderer.hpp"
#include "GraphicsInstance.hpp"
#include <typeinfo>
#include "Modules/ClassTypeId.hpp"

#include "Pipelines/BasePipeline.hpp"
#include "Pipelines/ShaderPipeline.hpp"
#include "Pipelines/ComputePipeline.hpp"

#include <iostream>

#include "Mesh.hpp"

namespace Soon
{
	GraphicsRenderer *GraphicsRenderer::_instance = nullptr;

	GraphicsRenderer *GraphicsRenderer::GetInstance(void)
	{
		if (!_instance)
			_instance = new GraphicsRenderer;

		return (_instance);
	}

	void GraphicsRenderer::ReleaseInstance(void)
	{
		if (_instance)
		{
			delete _instance;
			_instance = nullptr;
		}
	}

	void GraphicsRenderer::Initialize(void)
	{
		for (ShaderPipeline *pip : _graphicPipelines)
			pip = nullptr;
		for (ComputePipeline *pip : _computePipelines)
			pip = nullptr;
	}

	GraphicsRenderer::GraphicsRenderer(void) : _changes(false)
	{
		//_instance = this;
	}

	GraphicsRenderer::~GraphicsRenderer(void)
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();
		vkDeviceWaitIdle(device);
		RemoveAllPipelines();

		for (uint32_t index = 0; index < _meshs.size(); index++)
		{
			if (_meshs[index].count > 0)
			{
				// VERTEX
				vmaDestroyBuffer(GraphicsInstance::GetInstance()->GetAllocator(), _meshs[index].bufferRenderer.vertex.buffer, _meshs[index].bufferRenderer.vertex.bufferMemory);
				// INDICE
				vmaDestroyBuffer(GraphicsInstance::GetInstance()->GetAllocator(), _meshs[index].bufferRenderer.indices.buffer, _meshs[index].bufferRenderer.indices.bufferMemory);
			}
		}
	}

	void GraphicsRenderer::DestroyAllGraphicsPipeline( void )
	{
		std::cout << "Renderer : Destroy All Graphics Pipelines" << std::endl;

		for (ShaderPipeline *bp : _graphicPipelines)
		{
			if (bp)
				bp->DestroyGraphicPipeline();
		}
		for (ComputePipeline *bp : _computePipelines)
		{
			if (bp)
				bp->DestroyGraphicPipeline();
		}
	}

	template <typename T>
	void GraphicsRenderer::RemovePipeline(void)
	{
		_createdPipeline[ClassTypeId<BasePipeline>::GetId<T>()] = false;
		delete _graphicPipelines[ClassTypeId<BasePipeline>::GetId<T>()];

		_graphicPipelines[ClassTypeId<BasePipeline>::GetId<T>()] = nullptr;
	}

	void GraphicsRenderer::RemoveAllPipelines(void)
	{
		std::cout << "Renderer : Remove All Pipelines" << std::endl;

		for (ShaderPipeline *bp : _graphicPipelines)
		{
			if (bp)
			{
				delete bp;
				bp = nullptr;
			}
		}
		for (ComputePipeline *bp : _computePipelines)
		{
			if (bp)
			{
				delete bp;
				bp = nullptr;
			}
		}
	}

	void GraphicsRenderer::RecreateAllUniforms(void)
	{
		std::cout << "Renderer : Recreate All Uniforms" << std::endl;

		for (ShaderPipeline *bp : _graphicPipelines)
			if (bp)
				bp->RecreateUniforms();
		for (ComputePipeline *bp : _computePipelines)
			if (bp)
				bp->RecreateUniforms();
	}

	void GraphicsRenderer::RecreateAllPipelines(void)
	{
		std::cout << "Renderer : Recreate All Pipelines" << std::endl;

		for (ShaderPipeline *bp : _graphicPipelines)
			if (bp)
				bp->RecreatePipeline();
		for (ComputePipeline *bp : _computePipelines)
			if (bp)
				bp->RecreatePipeline();
	}

	void GraphicsRenderer::UpdateAllDatas(uint32_t imageIndex)
	{
		//std::cout << "Renderer : Update All Datas" << std::endl;
		for (ShaderPipeline *bp : _graphicPipelines)
			if (bp)
				bp->UpdateData(imageIndex);
		for (ComputePipeline *bp : _computePipelines)
			if (bp)
				bp->UpdateData(imageIndex);
	}

	void GraphicsRenderer::GraphicPipelinesBindCaller(VkCommandBuffer commandBuffer, uint32_t index)
	{
		for (ShaderPipeline *bp : _graphicPipelines)
		{
			if (bp != nullptr)
			{
				std::cout << "Graphic Bind Caller" << std::endl;
				bp->BindCaller(commandBuffer, index);
			}
		}
		std::cout << std::endl;
	}

	void GraphicsRenderer::ComputePipelinesBindCaller(VkCommandBuffer commandBuffer, uint32_t index)
	{
		for (ComputePipeline *bp : _computePipelines)
		{
			if (bp)
			{
				std::cout << "Compute Bind Caller" << std::endl;
				bp->BindCaller(commandBuffer, index);
			}
		}
		std::cout << std::endl;
	}

	void GraphicsRenderer::HasChange(void)
	{
		_changes = true;
	}

	bool GraphicsRenderer::IsChange(void)
	{
		return (_changes);
	}

	void GraphicsRenderer::ResetChange(void)
	{
		_changes = false;
	}

	bool GraphicsRenderer::IsValidMeshId( uint32_t id )
	{
		return (id < _meshCounter && _meshs[id].count > 0);
	}

	uint32_t GraphicsRenderer::AddMesh(Mesh *mesh, uint32_t meshId)
	{
		if (meshId != Soon::IdError)
		{
			if (IsValidMeshId(meshId))
			{
				_meshs[meshId].count += 1;
				return (meshId);
			}
			// TODO: ERROR
		}

		// Alloc Mesh
		if (!_freeId.empty())
		{
			meshId = _freeId.back();
			_freeId.pop_back();
		}
		else
		{
			meshId = _meshCounter;
			++_meshCounter;
			_meshs.resize(_meshCounter);
		}

		MeshRenderer mr;
		mr.count = 0;
		mr.bufferRenderer.vertex = GraphicsInstance::GetInstance()->CreateVertexBuffer(mesh->mVertexData, mesh->mVertexTotalSize);
		mr.bufferRenderer.indices = GraphicsInstance::GetInstance()->CreateIndexBuffer(mesh->mIndices, mesh->mNumIndices);
		_meshs[meshId] = mr;

		return (meshId);
	}

	void GraphicsRenderer::RemoveMesh(uint32_t meshId)
	{
		if (meshId == Soon::IdError || !IsValidMeshId(meshId))
			; // TODO: Error

		_meshs[meshId].count -= 1;
		if (_meshs[meshId].count == 0)
		{
			// VERTEX
			vmaDestroyBuffer(GraphicsInstance::GetInstance()->GetAllocator(), _meshs[meshId].bufferRenderer.vertex.buffer, _meshs[meshId].bufferRenderer.vertex.bufferMemory);
			// INDICE
			vmaDestroyBuffer(GraphicsInstance::GetInstance()->GetAllocator(), _meshs[meshId].bufferRenderer.indices.buffer, _meshs[meshId].bufferRenderer.indices.bufferMemory);

			_freeId.push_back(meshId);
		}
	}

	MeshBufferRenderer &GraphicsRenderer::GetMesh(uint32_t id)
	{
		return (_meshs[id].bufferRenderer);
	}

	void GraphicsRenderer::DestroyAllUniforms(void)
	{
		std::cout << "Renderer : Destroy All Uniforms" << std::endl;
		for (ShaderPipeline *bp : _graphicPipelines)
			if (bp)
				bp->DestroyAllUniforms();
		for (ComputePipeline *bp : _computePipelines)
			if (bp)
				bp->DestroyAllUniforms();
	}
} // namespace Soon
