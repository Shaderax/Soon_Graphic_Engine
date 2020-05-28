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
		bufferInfo.size = m_UniqueSize + (m_NonUniqueSize * m_MinElements);
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
 
		for ( uint32_t index = 0 ; index < GraphicsInstance::GetInstance()->GetSwapChainSize() ; index++)
			vmaCreateBuffer(GraphicsInstance::GetInstance()->GetAllocator(), &bufferInfo, &allocInfo, &(m_GpuBuffer[index]), &(m_GpuMemory[index]), nullptr);

		// UNIQUE
		uint32_t offset = 0;
		for (uint32_t index = 0 ; index < m_UniqueSets.size() ; index++)
		{
			std::vector<VkDescriptorSet> descriptorSets;
			descriptorSets = GraphicsInstance::GetInstance()->AllocateDescriptorSet(_descriptorSetLayout[m_Sets[index].set]);

			GraphicsInstance::GetInstance()->UpdateDescriptorSets(m_Sets[index],
																	offset,
																	descriptorSets.data(),
																	m_GpuBuffer.data(),
																	0);
			uint32_t id = 0;
			GraphicsInstance::GetInstance()->UpdateImageDescriptorSets(
																	&id,
																	1,
																	m_Sets[index],
																	descriptorSets.data());
			for ( uint32_t descriptorId = 0 ; descriptorId < descriptorSets.size() ; descriptorId++)
				m_DescriptorSets[descriptorId][index] = descriptorSets[descriptorId];

			offset += m_UniqueSets[index].size;
		}

		// NON UNIQUE
		offset = 0;
		for (uint32_t index = 0 ; index < m_Sets.size() ; index++)
		{
			for ( auto it = toDraw.begin() ; it != toDraw.end() ; ++it )
			{
				std::vector<VkDescriptorSet> descriptorSets;
				descriptorSets = GraphicsInstance::GetInstance()->AllocateDescriptorSet(_descriptorSetLayout[m_Sets[index].set]);

				GraphicsInstance::GetInstance()->UpdateDescriptorSets(m_Sets[index],
																		offset + m_UniqueSize + (it->second * m_NonUniqueSize),
																		descriptorSets.data(),
																		m_GpuBuffer.data(),
																		0);
				GraphicsInstance::GetInstance()->UpdateImageDescriptorSets(
																		&it->second,
																		1,
																		m_Sets[index],
																		descriptorSets.data());
				for ( uint32_t descriptorId = 0 ; descriptorId < descriptorSets.size() ; descriptorId++)
					m_DescriptorSets[descriptorId][m_UniqueSets.size() + (m_Sets.size() * it->second) + index] = descriptorSets[descriptorId];

				offset += m_Sets[index].size;
			}
		}
	}

	void UniformsBufferManager::AddUniform(Uniform uniform)
	{
		if (m_CpuBuffer != nullptr)
			; // TODO: Error
		
		m_NonUniqueSize += uniform._size;
		for (uint32_t index = 0 ; index < m_Sets.size() ; index++)
		{
			if (uniform._set == m_Sets[index].set)
			{
				m_Sets[index].uniforms.push_back(uniform);
				m_Sets[index].size += uniform._size;

				return ;
			}
		}

		DescriptorSetDescription description;
		description.set = uniform._set;
		description.size = uniform._size;
		description.uniforms.push_back(uniform);
		m_Sets.push_back(description);
	}
	
	void UniformsBufferManager::AddUniform( UniformTexture uniform )
	{
		if (m_CpuBuffer != nullptr)
			; // TODO: Error

		for (uint32_t index = 0 ; index < m_Sets.size() ; index++)
		{
			if (uniform._set == m_Sets[index].set)
			{
				m_Sets[index].uniformsTexture.push_back(uniform);
				return ;
			}
		}

		DescriptorSetDescription description;
		description.set = uniform._set;
		description.uniformsTexture.push_back(uniform);
		m_Sets.push_back(description);
	}

	void UniformsBufferManager::AddUniqueUniform( Uniform uniform )
	{
		m_UniqueSize += uniform._size;
		for (uint32_t index = 0 ; index < m_UniqueSets.size() ; index++)
		{
			if (uniform._set == m_UniqueSets[index].set)
			{
				m_UniqueSets[index].uniforms.push_back(uniform);
				m_UniqueSets[index].size += uniform._size;

				return ;
			}
		}

		DescriptorSetDescription description;
		description.set = uniform._set;
		description.size = uniform._size;
		description.uniforms.push_back(uniform);
		m_UniqueSets.push_back(description);
	}

	void UniformsBufferManager::AddUniqueUniform( UniformTexture uniform )
	{
		for (uint32_t index = 0 ; index < m_UniqueSets.size() ; index++)
		{
			if (uniform._set == m_UniqueSets[index].set)
			{
				m_UniqueSets[index].uniformsTexture.push_back(uniform);
				return ;
			}
		}

		DescriptorSetDescription description;
		description.set = uniform._set;
		description.uniformsTexture.push_back(uniform);
		m_UniqueSets.push_back(description);
	}

	void UniformsBufferManager::InitBuffers(void)
	{
		uint32_t swapChainSize = GraphicsInstance::GetInstance()->GetSwapChainSize();
		VmaAllocator& allocator = GraphicsInstance::GetInstance()->GetAllocator();

		m_GpuMemory.resize(swapChainSize);
		m_GpuBuffer.resize(swapChainSize);

		m_CpuBuffer = new uint8_t[m_UniqueSize + (m_NonUniqueSize * m_MinElements)]();

		// Buffer Creation
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = m_UniqueSize + (m_NonUniqueSize * m_MinElements);
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		//
		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

		for ( uint32_t index = 0 ; index < swapChainSize ; index++)
			vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &(m_GpuBuffer[index]), &(m_GpuMemory[index]), nullptr);

		m_DescriptorSets.resize(swapChainSize);

		for ( auto& descriptor : m_DescriptorSets )
			descriptor.resize(m_UniqueSets.size() + (m_Sets.size() * m_MinElements));

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
			for (uint32_t setId = 0 ; setId < m_Sets.size() ; setId++)
			{
				for (uint32_t index = 0; index < m_Sets[setId].uniforms.size() ; index++)
				{
					if (m_Sets[setId].uniforms[index]._name == name)
					{
						std::cout << m_Sets[setId].uniforms[index]._name << std::endl;
						memcpy(m_CpuBuffer + m_UniqueSize + offset + (m_NonUniqueSize * matId), value, m_Sets[setId].uniforms[index]._size);
						return;
					}
					offset += m_Sets[setId].uniforms[index]._size;
				}
			}
		}
		else
		{
			std::string varName;
			varName = name.substr(pos + 1);

			for (uint32_t setId = 0 ; setId < m_Sets.size() ; setId++)
			{
				for (uint32_t index = 0; index < m_Sets[setId].uniforms.size() ; index++)
				{
					if (m_Sets[setId].uniforms[index]._name == name.substr(0, pos))
					{
						for (uint32_t member = 0; member < m_Sets[setId].uniforms[index]._members.size(); member++)
						{
							if (varName == m_Sets[setId].uniforms[index]._members[member]._name)
							{
								memcpy(m_CpuBuffer + m_UniqueSize + offset + (m_NonUniqueSize * matId), value, m_Sets[setId].uniforms[index]._members[member]._size);
								std::cout << "Found Uniform : " << name.substr(0, pos) + "." << varName << std::endl;
							}
							offset += m_Sets[setId].uniforms[index]._members[member]._size;
						}
						return;
					}
				}
			}
		}
	}

	void UniformsBufferManager::SetTexture(std::string name, uint32_t idMat, uint32_t textureId)
	{
		for (uint32_t setId = 0 ; setId < m_Sets.size() ; setId++)
		{
			for (uint32_t index = 0 ; index < m_Sets[setId].uniformsTexture.size() ; index++)
			{
				if (name == m_Sets[setId].uniformsTexture[index]._name)
				{
					if (m_Sets[setId].uniformsTexture[index]._textureId[idMat] == Soon::IdError)
						GraphicsRenderer::GetInstance()->RemoveTexture(m_Sets[setId].uniformsTexture[index]._textureId[idMat]);
					GraphicsRenderer::GetInstance()->AddTexture(textureId);
					m_Sets[setId].uniformsTexture[index]._textureId[idMat] = textureId;
				}
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
		memcpy(data, m_CpuBuffer, m_UniqueSize + (m_NonUniqueSize * m_NumElements));
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
		for (uint32_t index = 0 ; index < m_Sets.size() ; index++)
		{
			for (uint32_t uniTexId = 0 ; uniTexId < m_Sets[index].uniformsTexture.size() ; uniTexId++)
			{
				if (idMat >= m_Sets[index].uniformsTexture[uniTexId]._textureId.size())
					m_Sets[index].uniformsTexture[uniTexId]._textureId.push_back(Soon::IdError);
			}
			std::vector<VkDescriptorSet> descriptorSets;
			descriptorSets = GraphicsInstance::GetInstance()->AllocateDescriptorSet(_descriptorSetLayout[m_Sets[index].set]);

			GraphicsInstance::GetInstance()->UpdateDescriptorSets(m_Sets[index],
																	m_UniqueSize + (idMat * m_NonUniqueSize) + offset,
																	descriptorSets.data(),
																	m_GpuBuffer.data(),
																	0);

			GraphicsInstance::GetInstance()->UpdateImageDescriptorSets(
																	&idMat,
																	1,
																	m_Sets[index],
																	descriptorSets.data());

			for ( uint32_t descriptorId = 0 ; descriptorId < descriptorSets.size() ; descriptorId++)
				m_DescriptorSets[descriptorId][m_UniqueSets.size() + (m_Sets.size() * idMat) + index] = descriptorSets[descriptorId];
			offset += m_Sets[index].size;
		}
		//
		m_NumElements = (m_NumElements < idMat + 1) ? idMat + 1 : m_NumElements;
	}

	void UniformsBufferManager::Free( uint32_t idMat )
	{
		/*
		// TODO:
		uint32_t offset = 0;
		uint32_t uniqueUniformsSize = m_UniqueUniforms.size() + m_UniqueUniformsTexture.size();
		uint32_t totalUniformsSize = m_UniformsTexture.size() + m_Uniforms.size();

		GraphicsInstance::GetInstance()->GetSwapChainSize();

		for ( uint32_t descriptorId = 0 ; descriptorId < GraphicsInstance::GetInstance()->GetSwapChainSize() ; descriptorId++)
		{
			std::vector<VkDescriptorSet> descriptorSets;

			for (uint32_t index = 0; index < m_Uniforms.size(); index++)
				GraphicsInstance::GetInstance()->DestroyDescriptorSet(m_DescriptorSets[descriptorId][uniqueUniformsSize + totalUniformsSize * idMat + index]);

			for (uint32_t index = 0; index < m_UniformsTexture.size(); index++)
				GraphicsInstance::GetInstance()->DestroyDescriptorSet(m_DescriptorSets[descriptorId][uniqueUniformsSize + totalUniformsSize * idMat + m_Uniforms.size() + index]);
		}
		*/
	}

	void UniformsBufferManager::AddLayoutBinding(VkDescriptorSetLayoutBinding ubo, uint32_t set)
	{
		if (uboLayoutBinding.size() <= set)
			uboLayoutBinding.resize(set + 1);
		uboLayoutBinding[set].push_back(ubo);
	}

	std::vector<VkDescriptorSetLayout> UniformsBufferManager::CreateDescriptorSetLayout( void )
	{
		for (uint32_t index = 0 ; index < uboLayoutBinding.size() ; index++)
		{
			std::cout << "Set : " << index << std::endl;
			for (uint32_t index2 = 0 ; index2 < uboLayoutBinding[index].size() ; index2++)
			{
				std::cout << "Binding : " << uboLayoutBinding[index][index2].binding << std::endl;
				std::cout << "descriptorCount : " << uboLayoutBinding[index][index2].descriptorCount << std::endl;
			}
		}
		_descriptorSetLayout = GraphicsInstance::GetInstance()->CreateDescriptorSetLayout(uboLayoutBinding);
		return _descriptorSetLayout;
	}

	std::vector<VkDescriptorSetLayout> UniformsBufferManager::GetDescriptorSetLayout( void )
	{
		return _descriptorSetLayout;
	}

	std::vector<DescriptorSetDescription>& UniformsBufferManager::GetSets( void )
	{
		return m_Sets;
	}

	std::vector<DescriptorSetDescription>& UniformsBufferManager::GetUniqueSets( void )
	{
		return m_UniqueSets;
	}

	std::vector<VkDescriptorSet>& UniformsBufferManager::GetDescriptorSet( uint32_t image )
	{
		return m_DescriptorSets[image];
	}
}; // namespace Soon
