#include "GraphicsRenderer.hpp"
#include "GraphicsInstance.hpp"
#include <typeinfo>
#include "Modules/ClassTypeId.hpp"

#include "Pipelines/BasePipeline.hpp"
#include "Pipelines/GraphicPipeline.hpp"
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

	GraphicsRenderer* GraphicsRenderer::GetInstanceOrNull(void)
	{
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
		m_DefaultTexture = new Texture(2, 2, EnumTextureFormat::RGBA, EnumTextureType::TEXTURE_2D);
		m_DefaultTexture->SetPixel(0, 0);
		m_DefaultTexture->SetPixel(0, 1);
		m_DefaultTexture->SetPixel(1, 0);
		m_DefaultTexture->SetPixel(1, 1);
		AddTexture(*m_DefaultTexture);
		//AddTexture(m_DefaultTexture->GetId());
		std::cout << "Default Texture Id : " << m_DefaultTexture->GetId() << std::endl;
		//delete m_DefaultTexture;
		//m_DefaultTexture = nullptr;
	}

	GraphicsRenderer::GraphicsRenderer(void) : _changes(false)
	{
		//_instance = this;
	}

	void GraphicsRenderer::RecreatePipelines( void )
	{
		//if (m_PipelinesToRecreate.size() != 0)
		//	vkDeviceWaitIdle(GraphicsInstance::GetInstance()->GetDevice());
		for ( ; !m_PipelinesToRecreate.empty() ; )
		{
			std::string name = m_PipelinesToRecreate.back();
			m_PipelinesToRecreate.pop_back();
			if (_computePipelines.find(name) != _computePipelines.end())
			{
				_computePipelines[name]->DestroyPipeline();
				_computePipelines[name]->RecreatePipeline();
				return ;
			}
			else if (_graphicPipelines.find(name) != _graphicPipelines.end())
			{
				std::cout << name << " Is recreared" << std::endl;
				_graphicPipelines[name]->DestroyPipeline();
				_graphicPipelines[name]->RecreatePipeline();
				return ;
			}
			else if (m_UniqueComputePipelines.find(name) != m_UniqueComputePipelines.end())
			{
				m_UniqueComputePipelines[name]->DestroyPipeline();
				m_UniqueComputePipelines[name]->RecreatePipeline();
				return ;
			}
			// TODO: else
		}
	}

	void GraphicsRenderer::Update( void )
	{
		if (!IsChange())
			return ;

		vkDeviceWaitIdle(GraphicsInstance::GetInstance()->GetDevice());
		RecreatePipelines();
		DestroyInvalids();
		GraphicsInstance::GetInstance()->FillCommandBuffer();
		ResetChange();
	}

	GraphicsRenderer::~GraphicsRenderer(void)
	{
		std::cout << "Destroy GraphicsRenderer" << std::endl;
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();
		vkDeviceWaitIdle(device);

		RemoveAllPipelines();

		DestroyInvalids();

		for (uint32_t index = 0; index < _meshs.size(); index++)
		{
			if (_meshs[index].count > 0)
			{
				std::cout << "Destroyed Mesh: " << index << std::endl;

				vmaDestroyBuffer(GraphicsInstance::GetInstance()->GetAllocator(), _meshs[index].bufferRenderer.vertex.buffer,
																					_meshs[index].bufferRenderer.vertex.bufferMemory);
				vmaDestroyBuffer(GraphicsInstance::GetInstance()->GetAllocator(), _meshs[index].bufferRenderer.indices.buffer,
																					_meshs[index].bufferRenderer.indices.bufferMemory);
			}
			else
			{
				std::cout << "NOT Destroyed Mesh: " << index << "count : " << _meshs[index].count << std::endl;
			}
		}
		
		for (uint32_t index = 0; index < m_Textures.size(); index++)
		{
			if (m_Textures[index].count > 0)
			{
				std::cout << "Destroyed Texture: " << index << std::endl;

				vmaDestroyImage(GraphicsInstance::GetInstance()->GetAllocator(), m_Textures[index].imageRenderer._textureImage,
																				m_Textures[index].imageRenderer._textureImageMemory);
				vkDestroySampler(GraphicsInstance::GetInstance()->GetDevice(), m_Textures[index].image._textureSampler, nullptr);
				vkDestroyImageView(GraphicsInstance::GetInstance()->GetDevice(), m_Textures[index].image._imageView, nullptr);
			}
		}

		for (uint32_t index = 0 ; index < m_Buffers.size() ; index++)
		{
			if (m_Buffers[index].count > 0)
			{
				std::cout << "Destroyed Buffer: " << index << std::endl;
				vmaDestroyBuffer(GraphicsInstance::GetInstance()->GetAllocator(), m_Buffers[index].GetBuffer(), m_Buffers[index].GetAllocation());
			}
			else
			{
				std::cout << "NOT Destroyed Buffer: " << index << "count : " << m_Buffers[index].count << std::endl;
			}
		}
	}

	void GraphicsRenderer::DestroyInvalids( void )
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

		vkDeviceWaitIdle(device);

		while (!m_MeshToSupress.empty())
		{
			uint32_t meshId = m_MeshToSupress.back();
			m_MeshToSupress.pop_back();

			std::cout << "INVALID Destroyed Mesh: " << meshId << std::endl;

			vmaDestroyBuffer(GraphicsInstance::GetInstance()->GetAllocator(), _meshs[meshId].bufferRenderer.vertex.buffer, _meshs[meshId].bufferRenderer.vertex.bufferMemory);
			vmaDestroyBuffer(GraphicsInstance::GetInstance()->GetAllocator(), _meshs[meshId].bufferRenderer.indices.buffer, _meshs[meshId].bufferRenderer.indices.bufferMemory);

			_freeId.push_back(meshId);
		}
		while (!m_TextureToSupress.empty())
		{
			uint32_t textureId = m_TextureToSupress.back();
			m_TextureToSupress.pop_back();

			std::cout << "Destroyed Texture: " << textureId << std::endl;

			vmaDestroyImage(GraphicsInstance::GetInstance()->GetAllocator(), m_Textures[textureId].imageRenderer._textureImage, m_Textures[textureId].imageRenderer._textureImageMemory);
			vkDestroySampler(GraphicsInstance::GetInstance()->GetDevice(), m_Textures[textureId].image._textureSampler, nullptr);
			vkDestroyImageView(GraphicsInstance::GetInstance()->GetDevice(), m_Textures[textureId].image._imageView, nullptr);

			m_FreeTextureId.push_back(textureId);
		}
		while (!m_BufferToSupress.empty())
		{
			uint32_t bufferId = m_BufferToSupress.back();
			m_BufferToSupress.pop_back();

			std::cout << "Destroyed Buffer: " << bufferId << std::endl;
			vmaDestroyBuffer(GraphicsInstance::GetInstance()->GetAllocator(), m_Buffers[bufferId].GetBuffer(), m_Buffers[bufferId].GetAllocation());

			m_BufferFreeId.push_back(bufferId);
		}
	}

	void GraphicsRenderer::SetProcessFrequency( std::string name, EProcessFrequency frequency )
	{
		if (frequency == EProcessFrequency::FRAME)
		{
			if (m_UniqueComputePipelines.find(name) != m_UniqueComputePipelines.end())
			{
				_computePipelines[name] = m_UniqueComputePipelines[name];
				m_UniqueComputePipelines.erase(name);
				HasChange();
				return ;
			}
		}
		else if (frequency == EProcessFrequency::ONCE)
		{
			if (_computePipelines.find(name) != _computePipelines.end())
			{
				m_UniqueComputePipelines[name] = _computePipelines[name];
				_computePipelines.erase(name);
				HasChange();
				return ;
			}
		}
	}

	void GraphicsRenderer::DestroyAllPipelines(void)
	{
		std::cout << "Renderer : Destroy All Graphics Pipelines" << std::endl;

		for( auto const& [key, val] : _graphicPipelines )
		{
			if (val)
				val->DestroyPipeline();
		}
		for( auto const& [key, val] : _computePipelines )
		{
			if (val)
				val->DestroyPipeline();
		}
		for( auto const& [key, val] : m_UniqueComputePipelines)
		{
			if (val)
				val->DestroyPipeline();
		}
	}

	void GraphicsRenderer::RemovePipeline(std::string pipeline)
	{
		if (_graphicPipelines.find(pipeline) != _graphicPipelines.end())
		{
			delete _graphicPipelines[pipeline];
			_graphicPipelines.erase(pipeline);
		}
		else if (_computePipelines.find(pipeline) != _computePipelines.end())
		{
			delete _computePipelines[pipeline];
			_computePipelines.erase(pipeline);
		}
		else if (m_UniqueComputePipelines.find(pipeline) != m_UniqueComputePipelines.end())
		{
			delete m_UniqueComputePipelines[pipeline];
			m_UniqueComputePipelines.erase(pipeline);
		}
		// TODO: HERE
	}

	void GraphicsRenderer::AddPipelineToRecreate( std::string name )
	{
		HasChange();
		m_PipelinesToRecreate.push_back(name);
	}

	void GraphicsRenderer::RemoveAllPipelines(void)
	{
		std::cout << "Renderer : Remove All Pipelines" << std::endl;

		for( auto const& [key, val] : _graphicPipelines )
			delete val;
		for( auto const& [key, val] : _computePipelines )
			delete val;
		for( auto const& [key, val] : m_UniqueComputePipelines )
			delete val;

		_graphicPipelines.clear();
		_computePipelines.clear();
		m_UniqueComputePipelines.clear();
	}

	void GraphicsRenderer::RecreateAllUniforms(void)
	{
		std::cout << "Renderer : Recreate All Uniforms" << std::endl;

		for( auto const& [key, val] : _graphicPipelines )
			val->RecreateUniforms();
		for( auto const& [key, val] : _computePipelines )
			val->RecreateUniforms();
		for( auto const& [key, val] : m_UniqueComputePipelines )
			val->RecreateUniforms();
	}

	void GraphicsRenderer::RecreateAllPipelines(void)
	{
		std::cout << "Renderer : Recreate All Pipelines" << std::endl;

		for( auto const& [key, val] : _graphicPipelines )
				val->RecreatePipeline();
		for( auto const& [key, val] : _computePipelines )
				val->RecreatePipeline();
		for( auto const& [key, val] : m_UniqueComputePipelines )
				val->RecreatePipeline();
	}

	void GraphicsRenderer::UpdateAllDatas(uint32_t imageIndex)
	{
		//std::cout << "Renderer : Update All Datas" << std::endl;
		for( auto const& [key, val] : _graphicPipelines )
				val->UpdateData(imageIndex);
		for( auto const& [key, val] : _computePipelines )
				val->UpdateData(imageIndex);
		for( auto const& [key, val] : m_UniqueComputePipelines )
				val->UpdateData(imageIndex);
	}

	//NEW
	// Max pipelines reach
	//template <typename... Args>
	BasePipeline* GraphicsRenderer::AddPipeline(std::string name)/*, Args... args)*/
	{
		GraphicPipelinesIterator graphicPip = _graphicPipelines.find(name);
		if (graphicPip != _graphicPipelines.end())
			return graphicPip->second;
		ComputePipelinesIterator computePip = _computePipelines.find(name);
		if (computePip != _computePipelines.end())
			return computePip->second;
		
		PipelineConf* conf = ReadPipelineJson(name);
		BasePipeline* pipeline;
		if ((conf->GetType() == EPipelineType::COMPUTE))
		{
			pipeline = new ComputePipeline(reinterpret_cast<ComputePipelineConf*>(conf));
			_computePipelines[name] = reinterpret_cast<ComputePipeline*>(pipeline);
		}
		else if ((conf->GetType() == EPipelineType::GRAPHIC))
		{
			pipeline = new GraphicPipeline(reinterpret_cast<GraphicPipelineConf*>(conf));
			_graphicPipelines[name] = reinterpret_cast<GraphicPipeline*>(pipeline);
		}
		pipeline->Init();
		_changes = true;
		return (pipeline);
	}

	void GraphicsRenderer::GraphicPipelinesBindCaller(VkCommandBuffer commandBuffer, uint32_t index)
	{
		for( auto const& [key, val] : _graphicPipelines )
		{
			val->BindCaller(commandBuffer, index);
		}
	}

	void GraphicsRenderer::ComputePipelinesBindCaller(VkCommandBuffer commandBuffer, uint32_t index)
	{
		for( auto const& [key, val] : _computePipelines )
		{
			val->BindCaller(commandBuffer, index);
		}
	}

	void GraphicsRenderer::DestroyAllUniforms(void)
	{
		std::cout << "Renderer : Destroy All Uniforms" << std::endl;
		for( auto const& [key, val] : _graphicPipelines )
				val->DestroyAllUniforms();
		for( auto const& [key, val] : _computePipelines )
				val->DestroyAllUniforms();
		for( auto const& [key, val] : m_UniqueComputePipelines )
			val->DestroyAllUniforms();
	}

	/**
	 *  MESH
	 */
	bool GraphicsRenderer::IsValidMeshId(uint32_t id)
	{
		return ((id < _meshCounter) && (_meshs[id].count > 0));
	}

	uint32_t GraphicsRenderer::AddMesh(uint32_t meshId)
	{
		if (IsValidMeshId(meshId))
		{
			_meshs[meshId].count += 1;
			return (meshId);
		}
		// TODO: ERROR
		std::cout << "MeshId: " << meshId << ", Count: " << _meshs[meshId].count << std::endl;

		return (meshId);
	}

	uint32_t GraphicsRenderer::AddMesh(Mesh *mesh, uint32_t meshId)
	{
		if (IsValidMeshId(meshId))
		{
			_meshs[meshId].count += 1;
			return (meshId);
		}
		// TODO: ERROR

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
		// TODO: Delete mVertexData for all in Gpu
		mr.bufferRenderer.vertex = GraphicsInstance::GetInstance()->CreateVertexBuffer(mesh->mVertexData.get(), mesh->mVertexTotalSize);
		mr.bufferRenderer.indices = GraphicsInstance::GetInstance()->CreateIndexBuffer(mesh->mIndices.get(), mesh->mNumIndices);
		_meshs[meshId] = mr;

		std::cout << "MeshId: " << meshId << ", Count: " << _meshs[meshId].count << std::endl;

		return (meshId);
	}

	void GraphicsRenderer::RemoveMesh(uint32_t meshId)
	{
		if (!IsValidMeshId(meshId))
			return ; // TODO: Error

		_meshs[meshId].count -= 1;
		if (_meshs[meshId].count == 0)
			m_MeshToSupress.push_back(meshId);
		std::cout << "MeshId: " << meshId << ", Count: " << _meshs[meshId].count << std::endl;

	}

	MeshBufferRenderer& GraphicsRenderer::GetMesh(uint32_t id)
	{
		return (_meshs[id].bufferRenderer);
	}

	/**
	 *  TEXTURE
	 */
	bool GraphicsRenderer::IsValidTextureId(uint32_t id)
	{
		return ((id < m_TextureCounter) && (m_Textures[id].count > 0));
	}
	
	uint32_t GraphicsRenderer::AddTexture(Texture& texture)
	{
		if (texture.GetId() != Soon::IdError)
			return AddTexture(texture.GetId());

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
		mr.image._imageView = GraphicsInstance::GetInstance()->CreateImageView(mr.imageRenderer._textureImage, TextureFormatToVkFormat(texture.GetFormat()), VK_IMAGE_ASPECT_COLOR_BIT, TextureTypeToVkImageType(texture.GetType()));
		mr.image._textureSampler = GraphicsInstance::GetInstance()->CreateTextureSampler(texture);
		m_Textures[textureId] = mr;

		texture.m_UniqueId = textureId;

		std::cout << "TextureId: " << textureId << ", Count: " << m_Textures[textureId].count << std::endl;
		return (textureId);
	}
	
	uint32_t GraphicsRenderer::AddTexture(uint32_t textureId)
	{
		std::cout << "TextureId: " << textureId << ", Count: " << m_Textures[textureId].count << std::endl;

		if (IsValidTextureId(textureId))
		{
			m_Textures[textureId].count += 1;
			return (textureId);
		}
		// TODO: ERROR

		return (textureId);
	}

	void GraphicsRenderer::RemoveTexture(uint32_t textureId)
	{
		if (!IsValidTextureId(textureId))
			return ; // TODO: Error

		m_Textures[textureId].count -= 1;
		if (m_Textures[textureId].count == 0)
			m_TextureToSupress.push_back(textureId);
		std::cout << "TextureId: " << textureId << ", Count: " << m_Textures[textureId].count << std::endl;
	}

	ImageProperties& GraphicsRenderer::GetImageProperties(uint32_t id)
	{
		if (id == Soon::IdError)
			return m_Textures[m_DefaultTexture->GetId()].image;
		return (m_Textures[id].image);
	}

	void GraphicsRenderer::SetDefaultTexture(Texture& texture)
	{
		// TODO: HERE
		RemoveTexture(m_DefaultTexture->GetId());
		delete m_DefaultTexture;

		texture.m_UniqueId = AddTexture(texture);
		m_DefaultTexture = &texture;
	}

	/**
	 * GpuBuffer
	 */
	bool GraphicsRenderer::IsValidBufferId(uint32_t id)
	{
		return ((id < m_BufferCounter) && (m_Buffers[id].count > 0));
	}

	uint32_t GraphicsRenderer::AddBuffer(uint32_t bufferId)
	{
		std::cout << "AddBuffer without" << std::endl;
		if (IsValidBufferId(bufferId))
		{
			m_Buffers[bufferId].count += 1;
			std::cout << "\t BufferId: " << bufferId << ", Count: " << m_Buffers[bufferId].count << std::endl;

			return (bufferId);
		}
		// TODO: ERROR

		std::cout << "\t BufferId: " << bufferId << ", Count: " << m_Buffers[bufferId].count << std::endl;
		return (bufferId);
	}

	uint32_t GraphicsRenderer::AddBuffer(GpuBuffer& buffer)
	{
		uint32_t bufferId = 0;
		std::cout << "AddBuffer with" << std::endl;
		if (IsValidBufferId(buffer.GetId()))
		{
			m_Buffers[buffer.GetId()].count += 1;
			std::cout << "\t 2buffer.GetId(): " << buffer.GetId() << ", Count: " << m_Buffers[buffer.GetId()].count << std::endl;
			return (buffer.GetId());
		}

		// TODO: ERROR

		if (!m_BufferFreeId.empty())
		{
			bufferId = m_BufferFreeId.back();
			m_BufferFreeId.pop_back();

			BufferRenderer dataRenderer(buffer.GetBufferUsage(), buffer.GetMemoryUsage(), buffer.GetSize());

			dataRenderer.count = 1;
			m_Buffers[bufferId] = std::move(dataRenderer);

			std::cout << "\t 3BufferId: " << bufferId << ", Count: " << m_Buffers[bufferId].count << "Buffer: " << m_Buffers.back().GetBuffer() << std::endl;
		}
		else
		{
			bufferId = m_BufferCounter;
			++m_BufferCounter;
			m_Buffers.emplace_back(buffer.GetBufferUsage(), buffer.GetMemoryUsage(), buffer.GetSize());

			m_Buffers.at(bufferId).count = 1;
			std::cout << "\t 5BufferId: " << bufferId << ", Count: " << m_Buffers.at(bufferId).count << std::endl;
			return bufferId;
		}
		return (bufferId);
	}

	void GraphicsRenderer::RemoveBuffer(uint32_t bufferId)
	{
		std::cout << "RemoveBuffer: " << bufferId << ", valid?: " << IsValidBufferId(bufferId) << std::endl;
		if (!IsValidBufferId(bufferId))
			return ; // TODO: Error

		m_Buffers[bufferId].count -= 1;
		if (m_Buffers[bufferId].count == 0)
			m_BufferToSupress.push_back(bufferId);
		std::cout << "\t BufferId: " << bufferId << ", Count: " << m_Buffers[bufferId].count << std::endl;
	}

	BufferRenderer& GraphicsRenderer::GetBufferRenderer(uint32_t id)
	{
		//TODO: if id > counter for all Getter
		if (id == Soon::IdError)
			throw std::runtime_error("Id == IdError");
		return m_Buffers[id];
	}

	/**
	 */

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
