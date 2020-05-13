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
			// VERTEX
			vkDestroyBuffer(device, _meshs[index].vertex.buffer, nullptr);
			vkFreeMemory(device, _meshs[index].vertex.bufferMemory, nullptr);

			// INDICE
			vkDestroyBuffer(device, _meshs[index].indices.buffer, nullptr);
			vkFreeMemory(device, _meshs[index].indices.bufferMemory, nullptr);
		}
	}

	void GraphicsRenderer::DestroyAllGraphicsPipeline( void )
	{
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
		for (ShaderPipeline *bp : _graphicPipelines)
			if (bp)
				bp->RecreateUniforms();
		for (ComputePipeline *bp : _computePipelines)
			if (bp)
				bp->RecreateUniforms();
	}

	void GraphicsRenderer::RecreateAllPipelines(void)
	{
		for (ShaderPipeline *bp : _graphicPipelines)
			if (bp)
				bp->RecreatePipeline();
		for (ComputePipeline *bp : _computePipelines)
			if (bp)
				bp->RecreatePipeline();
	}

	void GraphicsRenderer::UpdateAllDatas(uint32_t imageIndex)
	{
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

	uint32_t GraphicsRenderer::AddMesh(Mesh *mesh)
	{
		uint32_t id;
		// Alloc Mesh
		if (!_freeId.empty())
		{
			id = _freeId.back();
			_freeId.pop_back();
		}
		else
		{
			id = _meshCounter;
			++_meshCounter;
			_meshs.resize(_meshCounter);
		}

		MeshBufferRenderer mbr;
		mbr.vertex = GraphicsInstance::GetInstance()->CreateVertexBuffer(mesh->mVertexData, mesh->mVertexTotalSize);
		mbr.indices = GraphicsInstance::GetInstance()->CreateIndexBuffer(mesh->mIndices, mesh->mNumIndices);
		_meshs[id] = mbr;

		return (id);
	}

	void GraphicsRenderer::RemoveMesh(uint32_t idMesh)
	{
	}

	MeshBufferRenderer &GraphicsRenderer::GetMesh(uint32_t id)
	{
		//std::cout << "Try to get mesh at id : " << id << std::endl;
		return (_meshs[id]);
	}

	void GraphicsRenderer::DestroyAllUniforms(void)
	{
		for (ShaderPipeline *bp : _graphicPipelines)
			if (bp)
				bp->DestroyAllUniforms();
		for (ComputePipeline *bp : _computePipelines)
			if (bp)
				bp->DestroyAllUniforms();
	}
} // namespace Soon
