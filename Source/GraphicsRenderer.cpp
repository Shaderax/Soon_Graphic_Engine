#include "GraphicsRenderer.hpp"
#include "GraphicsInstance.hpp"
#include <typeinfo>
#include "Modules/ClassTypeId.hpp"

#include "Pipelines/BasePipeline.hpp"
#include "Pipelines/ShaderPipeline.hpp"
#include "Pipelines/ComputePipeline.hpp"

#include <iostream>

#include "Mesh.hpp"
#include "Texture.hpp"

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
		// TODO: FIRST : REACTIVATE
		m_DefaultTexture = new Texture(2, 2, EnumTextureFormat::RGBA, EnumTextureType::TEXTURE_2D);
		m_DefaultTexture->SetPixel(0, 0);
		m_DefaultTexture->SetPixel(0, 1);
		m_DefaultTexture->SetPixel(1, 0);
		m_DefaultTexture->SetPixel(1, 1);
		AddTexture(m_DefaultTexture);
		std::cout << "Default Texture Id : " << m_DefaultTexture->GetId() << std::endl;

		for (ShaderPipeline *pip : _graphicPipelines)
			pip = nullptr;
		for (ComputePipeline *pip : _computePipelines)
			pip = nullptr;
	}

	GraphicsRenderer::GraphicsRenderer(void) : _changes(false)
	{
		//_instance = this;
	}

	void GraphicsRenderer::DestroyInvalids( void )
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();
		vkDeviceWaitIdle(device);

		for (uint32_t index = 0 ; index < m_MeshToSupress.size() ; index++)
		{
			// TODO: DEGUEU le idle
			//VkDevice device = GraphicsInstance::GetInstance()->GetDevice();
			//vkDeviceWaitIdle(device);

			uint32_t meshId = m_MeshToSupress.back();
			m_MeshToSupress.pop_back();

			vmaDestroyBuffer(GraphicsInstance::GetInstance()->GetAllocator(), _meshs[meshId].bufferRenderer.vertex.buffer, _meshs[meshId].bufferRenderer.vertex.bufferMemory);
			vmaDestroyBuffer(GraphicsInstance::GetInstance()->GetAllocator(), _meshs[meshId].bufferRenderer.indices.buffer, _meshs[meshId].bufferRenderer.indices.bufferMemory);

			_freeId.push_back(meshId);
		}
		for (uint32_t index = 0 ; index < m_TextureToSupress.size() ; index++)
		{
			uint32_t textureId = m_TextureToSupress.back();
			m_TextureToSupress.pop_back();

			vmaDestroyImage(GraphicsInstance::GetInstance()->GetAllocator(), m_Textures[textureId].imageRenderer._textureImage, m_Textures[textureId].imageRenderer._textureImageMemory);
			vkDestroySampler(GraphicsInstance::GetInstance()->GetDevice(), m_Textures[textureId].image._textureSampler, nullptr);
			vkDestroyImageView(GraphicsInstance::GetInstance()->GetDevice(), m_Textures[textureId].image._imageView, nullptr);

			m_FreeTextureId.push_back(textureId);	
		}
	}

	GraphicsRenderer::~GraphicsRenderer(void)
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();
		vkDeviceWaitIdle(device);
		RemoveAllPipelines();

		DestroyInvalids();

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
		
		for (uint32_t index = 0; index < m_Textures.size(); index++)
		{
			if (m_Textures[index].count > 0)
			{
				vmaDestroyImage(GraphicsInstance::GetInstance()->GetAllocator(), m_Textures[index].imageRenderer._textureImage, m_Textures[index].imageRenderer._textureImageMemory);
				vkDestroySampler(GraphicsInstance::GetInstance()->GetDevice(), m_Textures[index].image._textureSampler, nullptr);
				vkDestroyImageView(GraphicsInstance::GetInstance()->GetDevice(), m_Textures[index].image._imageView, nullptr);
			}
		}
	}

	void GraphicsRenderer::DestroyAllGraphicsPipeline(void)
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

	bool GraphicsRenderer::IsValidMeshId(uint32_t id)
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
		mr.count = 1;
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
			m_MeshToSupress.push_back(meshId);
	}

	MeshBufferRenderer& GraphicsRenderer::GetMesh(uint32_t id)
	{
		return (_meshs[id].bufferRenderer);
	}

	bool GraphicsRenderer::IsValidTextureId(uint32_t id)
	{
		return (id < m_TextureCounter && m_Textures[id].count > 0);
	}
	
	uint32_t GraphicsRenderer::AddTexture(Texture* texture)
	{
		if (texture->GetId() != Soon::IdError)
			return AddTexture(texture->GetId());

		uint32_t textureId;
		if (!m_FreeTextureId.empty())
		{
			textureId = m_FreeTextureId.back();
			m_FreeTextureId.pop_back();
		}
		else
		{
			textureId = m_TextureCounter;
			++m_TextureCounter;
			m_Textures.resize(m_TextureCounter);
		}

		TextureRenderer mr;
		mr.count = 1;
		mr.imageRenderer = GraphicsInstance::GetInstance()->CreateTextureImage(texture);
		mr.image._imageView = GraphicsInstance::GetInstance()->CreateImageView(mr.imageRenderer._textureImage, TextureFormatToVkFormat(texture->GetFormat()), VK_IMAGE_ASPECT_COLOR_BIT, TextureTypeToVkImageType(texture->GetType()));
		mr.image._textureSampler = GraphicsInstance::GetInstance()->CreateTextureSampler(texture);
		m_Textures[textureId] = mr;

		texture->m_UniqueId = textureId;

		return (textureId);
	}
	
	uint32_t GraphicsRenderer::AddTexture(uint32_t textureId)
	{
		if (textureId != Soon::IdError)
		{
			if (IsValidTextureId(textureId))
			{
				m_Textures[textureId].count += 1;
				return (textureId);
			}
			// TODO: ERROR
		}

		return (textureId);
	}

	void GraphicsRenderer::RemoveTexture(uint32_t textureId)
	{
		if (textureId == Soon::IdError || !IsValidTextureId(textureId))
			; // TODO: Error

		m_Textures[textureId].count -= 1;
		if (m_Textures[textureId].count == 0)
			m_TextureToSupress.push_back(textureId);
	}

	ImageProperties& GraphicsRenderer::GetImageProperties(uint32_t id)
	{
		if (id == Soon::IdError)
			return m_Textures[m_DefaultTexture->GetId()].image;
		return (m_Textures[id].image);
	}

	void GraphicsRenderer::SetDefaultTexture(Texture* texture)
	{

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
} // namespace Soon
