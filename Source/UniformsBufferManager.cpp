#include "UniformsBufferManager.hpp"
#include "GraphicsRenderer.hpp"
#include "Pipelines/BasePipeline.hpp"

namespace Soon
{
	UniformsBufferManager::UniformsBufferManager( void )
	{

	}

	UniformsBufferManager::~UniformsBufferManager( void )
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

		for (VkDescriptorSetLayout &dsl : _descriptorSetLayout)
			vkDestroyDescriptorSetLayout(device, dsl, nullptr);
	}

	void UniformsBufferManager::RecreateUniforms( std::unordered_map<uint32_t, uint32_t>& toDraw )
	{
		// Buffer Creation
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = m_SizeUniqueUniformBlock + (m_SizeUniformBlock * m_MinElements);
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
 
		for ( uint32_t index = 0 ; index < GraphicsInstance::GetInstance()->GetSwapChainSize() ; index++)
			vmaCreateBuffer(GraphicsInstance::GetInstance()->GetAllocator(), &bufferInfo, &allocInfo, &m_GpuBuffer[index], &(m_GpuMemory[index]), nullptr);

		uint32_t offset = 0;
		uint32_t uniqueUniformsSize = m_UniqueUniforms.size() + m_UniqueUniformsTexture.size();
		uint32_t totalUniformsSize = m_UniformsTexture.size() + m_Uniforms.size();
/*
		for (uint32_t index = 0; index < m_UniqueUniforms.size(); index++)
		{
			std::vector<VkDescriptorSet> descriptorSets;
			descriptorSets = GraphicsInstance::GetInstance()->CreateDescriptorSets(m_UniqueUniforms[index]._size,
																					m_UniqueUniforms[index]._binding,
																					offset,
																					_descriptorSetLayout[m_UniqueUniforms[index]._set],
																					m_GpuBuffer.data(),
																					0);				

			for ( uint32_t descriptorId = 0 ; descriptorId < descriptorSets.size() ; descriptorId++ )
				m_DescriptorSets[descriptorId][index] = descriptorSets[descriptorId];

			offset += m_Uniforms[index]._size;
		}

		for (uint32_t index = 0; index < m_UniqueUniformsTexture.size(); index++)
		{
			std::vector<VkDescriptorSet> descriptorSets;

			ImageProperties& imgProp = GraphicsRenderer::GetInstance()->GetImageProperties( m_UniqueUniformsTexture[index]._textureId[0] );
			descriptorSets = GraphicsInstance::GetInstance()->CreateImageDescriptorSets(imgProp._imageView, imgProp._textureSampler, _descriptorSetLayout[m_UniqueUniformsTexture[index]._set], m_UniqueUniformsTexture[index]._binding);				

			for ( uint32_t descriptorId = 0 ; descriptorId < descriptorSets.size() ; descriptorId++ )
				m_DescriptorSets[descriptorId][m_UniqueUniforms.size() + index] = descriptorSets[descriptorId];
		}
*/
		offset = 0;
		for (uint32_t index = 0; index < m_Uniforms.size(); index++)
		{
			for ( auto it = toDraw.begin() ; it != toDraw.end() ; ++it )
			{
				std::vector<VkDescriptorSet> descriptorSets;
				descriptorSets = GraphicsInstance::GetInstance()->CreateDescriptorSets(m_Uniforms[index]._size,
																						m_Uniforms[index]._binding,
																						offset + m_SizeUniqueUniformBlock + (m_SizeUniformBlock * it->second),
																						_descriptorSetLayout[m_Uniforms[index]._set],
																						m_GpuBuffer.data(),
																						0);				

				for ( uint32_t descriptorId = 0 ; descriptorId < descriptorSets.size() ; descriptorId++ )
					m_DescriptorSets[descriptorId][uniqueUniformsSize + totalUniformsSize * it->second + index] = descriptorSets[descriptorId];

				offset += m_Uniforms[index]._size;
			}
		}
/*
		offset = 0;
		for (uint32_t index = 0; index < m_UniformsTexture.size(); index++)
		{
			for ( auto it = toDraw.begin() ; it != toDraw.end() ; ++it )
			{
				std::vector<VkDescriptorSet> descriptorSets;
				ImageProperties& imgProp = GraphicsRenderer::GetInstance()->GetImageProperties( m_UniformsTexture[index]._textureId[it->second] );

				descriptorSets = GraphicsInstance::GetInstance()->CreateImageDescriptorSets(imgProp._imageView,
																							imgProp._textureSampler,
																							_descriptorSetLayout[m_UniformsTexture[index]._set],
																							m_UniformsTexture[index]._binding);

				for ( uint32_t descriptorId = 0 ; descriptorId < descriptorSets.size() ; descriptorId++ )
					m_DescriptorSets[descriptorId][uniqueUniformsSize + totalUniformsSize * it->second + index] = descriptorSets[descriptorId];

				offset += m_Uniforms[index]._size;
			}
		}
		*/
	}

	void UniformsBufferManager::AddUniform(Uniform uniform)
	{
		if (m_CpuBuffer != nullptr)
			; // TODO: Error
			
		m_SizeUniformBlock += uniform._size;
		m_Uniforms.push_back(uniform);
	}
	
	void UniformsBufferManager::AddUniform( UniformTexture uniform )
	{
		if (m_CpuBuffer != nullptr)
			; // TODO: Error

		m_UniformsTexture.push_back(uniform);
	}

	void UniformsBufferManager::AddUniqueUniform( Uniform uniform )
	{
		m_SizeUniqueUniformBlock += uniform._size;
		m_UniqueUniforms.push_back(uniform);
	}

	void UniformsBufferManager::AddUniqueUniform( UniformTexture uniform )
	{
		m_UniqueUniformsTexture.push_back(uniform);
	}

	void UniformsBufferManager::InitBuffers(void)
	{
		uint32_t swapChainSize = GraphicsInstance::GetInstance()->GetSwapChainSize();
		VmaAllocator& allocator = GraphicsInstance::GetInstance()->GetAllocator();

		m_GpuMemory.resize(swapChainSize);
		m_GpuBuffer.resize(swapChainSize);

		m_CpuBuffer = new uint8_t[m_SizeUniqueUniformBlock + (m_SizeUniformBlock * m_MinElements)];

		// Buffer Creation
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = m_SizeUniqueUniformBlock + (m_SizeUniformBlock * m_MinElements);
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		//
		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
 
		for ( uint32_t index = 0 ; index < swapChainSize ; index++)
			vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &m_GpuBuffer[index], &(m_GpuMemory[index]), nullptr);

		m_DescriptorSets.resize(swapChainSize);

		for ( auto& descriptor : m_DescriptorSets )
			descriptor.resize(((m_UniformsTexture.size() + m_Uniforms.size()) * m_MinElements) + m_UniqueUniformsTexture.size() + m_UniqueUniforms.size());

		m_ActualResize += 1;
	}

	void UniformsBufferManager::Set(std::string name, void *value, uint32_t matId )
	{
		// TODO: If we exced _uniformDataSize
		// TODO: Set UNIQUE
		// TODO: SETTEXTURE
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();
		uint32_t currentImage = GraphicsInstance::GetInstance()->GetNextIdImageToRender();

		uint32_t offset = 0;

		void *data = nullptr;

		size_t pos = name.find(".");
		if (pos == std::string::npos)
		{
			for (uint32_t index = 0; index < m_Uniforms.size(); index++)
			{
				std::cout << m_Uniforms[index]._name << std::endl;
				if (m_Uniforms[index]._name == name)
				{
					memcpy(m_CpuBuffer + m_SizeUniqueUniformBlock + offset + (m_SizeUniformBlock * matId), value, m_Uniforms[index]._size);
					return;
				}
				offset += m_Uniforms[index]._size;
			}
		}
		else
		{
			std::string varName;
			varName = name.substr(pos + 1);

			for (uint32_t index = 0; index < m_Uniforms.size(); index++)
			{
				if (m_Uniforms[index]._name == name.substr(0, pos))
				{
					for (uint32_t member = 0; member < m_Uniforms[index]._members.size(); member++)
					{
						if (varName == m_Uniforms[index]._members[member]._name)
						{
							memcpy(m_CpuBuffer + m_SizeUniqueUniformBlock + offset + (m_SizeUniformBlock * matId), value, m_Uniforms[index]._size);
							std::cout << "Found Uniform : " << name.substr(0, pos) + "." << varName << std::endl;
						}
						offset += m_Uniforms[index]._members[member]._size;
					}
					return;
				}
				offset += m_Uniforms[index]._size;
			}
		}
	}

	void UniformsBufferManager::DestroyAllUniforms(void)
	{
		// TODO: VkDescriptorSet
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

		for (uint32_t idChain = 0; idChain < GraphicsInstance::GetInstance()->GetSwapChainSize(); idChain++)
			vmaDestroyBuffer(GraphicsInstance::GetInstance()->GetAllocator(), m_GpuBuffer[idChain], m_GpuMemory[idChain]);
	}

	void UniformsBufferManager::UpdateToGPU( uint32_t currentImg )
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();
		VmaAllocator& allocator = GraphicsInstance::GetInstance()->GetAllocator();

		void* data = nullptr;

		vmaMapMemory(allocator, m_GpuMemory[currentImg], &data);
		memcpy(data, m_CpuBuffer, m_SizeUniqueUniformBlock + (m_SizeUniformBlock * m_NumElements));
		vmaUnmapMemory(allocator, m_GpuMemory[currentImg]);
	}

	void UniformsBufferManager::CheckAndResize( uint32_t num )
	{
	}

	void UniformsBufferManager::Allocate( uint32_t idMat )
	{
		// TODO:
		CheckAndResize(m_NumElements + 1);

		uint32_t offset = 0;
		for (uint32_t index = 0; index < m_Uniforms.size(); index++)
		{
			std::vector<VkDescriptorSet> descriptorSets;

			descriptorSets = GraphicsInstance::GetInstance()->CreateDescriptorSets(m_Uniforms[index]._size, m_Uniforms[index]._binding, offset + m_SizeUniqueUniformBlock + (m_SizeUniformBlock * idMat), _descriptorSetLayout[m_Uniforms[index]._set], m_GpuBuffer.data(), 0);
			for ( uint32_t descriptorId = 0 ; descriptorId < descriptorSets.size() ; descriptorId++)
			{
				// TODO: UniformTexture
				m_DescriptorSets[descriptorId][(m_Uniforms.size() * idMat) + index] = descriptorSets[descriptorId];
			}
			offset += m_Uniforms[index]._size;
		}

//		for (uint32_t index = 0; index < _uniformsTexture.size(); index++)
//		{
//		}

		m_NumElements = (m_NumElements < idMat + 1) ? idMat + 1 : m_NumElements;
	}

	void UniformsBufferManager::Free( uint32_t idMat )
	{
		uint32_t offset = 0;
		GraphicsInstance::GetInstance()->GetSwapChainSize();
		for (uint32_t index = 0; index < m_Uniforms.size(); index++)
		{
			std::vector<VkDescriptorSet> descriptorSets;

			for ( uint32_t descriptorId = 0 ; descriptorId < GraphicsInstance::GetInstance()->GetSwapChainSize() ; descriptorId++)
			{
				GraphicsInstance::GetInstance()->DestroyDescriptorSet(m_DescriptorSets[descriptorId][(m_Uniforms.size() * idMat) + index]);
				// TODO: UniformTexture
			}
		}
	}

	void UniformsBufferManager::AddLayoutBinding(VkDescriptorSetLayoutBinding ubo, uint32_t set)
	{
		if (uboLayoutBinding.size() <= set)
			uboLayoutBinding.resize(set + 1);
		uboLayoutBinding[set].push_back(ubo);
	}

	std::vector<VkDescriptorSetLayout> UniformsBufferManager::CreateDescriptorSetLayout( void )
	{
		_descriptorSetLayout = GraphicsInstance::GetInstance()->CreateDescriptorSetLayout(uboLayoutBinding);
		return _descriptorSetLayout;
	}

	std::vector<VkDescriptorSetLayout> UniformsBufferManager::GetDescriptorSetLayout( void )
	{
		return _descriptorSetLayout;
	}

	std::vector<Uniform>& UniformsBufferManager::GetNonUniqueUniforms( void )
	{
		return m_Uniforms;
	}

	std::vector<VkDescriptorSet>& UniformsBufferManager::GetDescriptorSet( uint32_t image )
	{
		return m_DescriptorSets[image];
	}


}; // namespace Soon
