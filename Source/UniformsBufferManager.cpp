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
		if (m_UniqueSize + (m_NonUniqueSize * m_MinElements) != 0)
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
		}

		// UNIQUE
		uint32_t offset = 0;
		for (uint32_t index = 0 ; index < m_UniqueSets.size() ; index++)
		{
			std::vector<VkDescriptorSet> descriptorSets;
			descriptorSets = GraphicsInstance::GetInstance()->AllocateDescriptorSet(_descriptorSetLayout[m_UniqueSets[index].set]);

			GraphicsInstance::GetInstance()->UpdateDescriptorSets(m_UniqueSets[index],
																	offset,
																	descriptorSets.data(),
																	m_GpuBuffer.data(),
																	0);
			uint32_t id = 0;
			GraphicsInstance::GetInstance()->UpdateImageDescriptorSets(&id,
																		1,
																		m_UniqueSets[index],
																		descriptorSets.data());

			GraphicsInstance::GetInstance()->UpdateRuntimeDescriptorSets(
																		&id,
																		1,
																		m_UniqueSets[index],
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

				GraphicsInstance::GetInstance()->UpdateRuntimeDescriptorSets(
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

	/**
	 *  ADDUNIFORM
	 */
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

	void UniformsBufferManager::AddUniform( UniformRuntime uniform )
	{
		if (m_CpuBuffer != nullptr)
			; // TODO: Error

		for (uint32_t index = 0 ; index < m_Sets.size() ; index++)
		{
			if (uniform.mSet == m_Sets[index].set)
			{
				m_Sets[index].uniformsRuntime.push_back(uniform);
				return ;
			}
		}

		DescriptorSetDescription description;
		description.set = uniform.mSet;
		description.uniformsRuntime.push_back(uniform);
		m_Sets.push_back(description);
	}

	void UniformsBufferManager::AddUniqueUniform( UniformRuntime uniform )
	{
		for (uint32_t index = 0 ; index < m_UniqueSets.size() ; index++)
		{
			if (uniform.mSet == m_UniqueSets[index].set)
			{
				m_UniqueSets[index].uniformsRuntime.push_back(uniform);
				m_UniqueSets[index].size += uniform.mSize;

				return ;
			}
		}

		DescriptorSetDescription description;
		description.set = uniform.mSet;
		description.size = uniform.mSize;
		description.uniformsRuntime.push_back(uniform);
		m_UniqueSets.push_back(description);
	}

	/**
	 * INIT
	 */
	void UniformsBufferManager::InitBuffers(void)
	{
		uint32_t swapChainSize = GraphicsInstance::GetInstance()->GetSwapChainSize();
		VmaAllocator& allocator = GraphicsInstance::GetInstance()->GetAllocator();

		if (m_UniqueSize + (m_NonUniqueSize * m_MinElements) != 0)
		{
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
				VkResult ret = vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &(m_GpuBuffer[index]), &(m_GpuMemory[index]), nullptr);
		}

		m_DescriptorSets.resize(swapChainSize);

		for ( auto& descriptor : m_DescriptorSets )
			descriptor.resize(m_UniqueSets.size() + (m_Sets.size() * m_MinElements));

		// UNIQUE
		uint32_t offset = 0;
		for (uint32_t index = 0 ; index < m_UniqueSets.size() ; index++)
		{
			for (uint32_t uniId = 0 ; uniId < m_UniqueSets[index].uniformsTexture.size() ; uniId++)
			{
				m_UniqueSets[index].uniformsTexture[uniId]._textureId.resize(1);
				m_UniqueSets[index].uniformsTexture[uniId]._textureId[0] = Soon::IdError;
			}
			for (uint32_t uniId = 0 ; uniId < m_UniqueSets[index].uniformsRuntime.size() ; uniId++)
			{
				m_UniqueSets[index].uniformsRuntime[uniId].mMembers.back().numInBuffer.push_back(1);
				m_UniqueSets[index].uniformsRuntime[uniId].mBuffers.reserve(1);
				m_UniqueSets[index].uniformsRuntime[uniId].mBuffers.emplace_back(VK_BUFFER_USAGE_TRANSFER_DST_BIT |
																					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
																					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
																					VMA_MEMORY_USAGE_GPU_ONLY,
																					GetUniformRuntimeSize(m_Sets[index].uniformsRuntime[uniId], 0));
			}

			std::vector<VkDescriptorSet> descriptorSets;
			descriptorSets = GraphicsInstance::GetInstance()->AllocateDescriptorSet(_descriptorSetLayout[m_UniqueSets[index].set]);

			GraphicsInstance::GetInstance()->UpdateDescriptorSets(m_UniqueSets[index],
																	offset,
																	descriptorSets.data(),
																	m_GpuBuffer.data(),
																	0);
			uint32_t id = 0;
			GraphicsInstance::GetInstance()->UpdateImageDescriptorSets(
																	&id,
																	1,
																	m_UniqueSets[index],
																	descriptorSets.data());

			GraphicsInstance::GetInstance()->UpdateRuntimeDescriptorSets(
																		&id,
																		1,
																		m_UniqueSets[index],
																		descriptorSets.data());

			for ( uint32_t descriptorId = 0 ; descriptorId < descriptorSets.size() ; descriptorId++)
				m_DescriptorSets[descriptorId][index] = descriptorSets[descriptorId];

			offset += m_UniqueSets[index].size;
		}

		m_ActualResize += 1;
	}

	void* UniformsBufferManager::Get( std::string name, uint32_t matId )
	{
	}

	void UniformsBufferManager::Set(std::string name, void *value, uint32_t matId )
	{
		// TODO: If we exced _uniformDataSize
		// TODO: Set UNIQUE
		// TODO: SETTEXTURE
		// TODO: Runtime
		// TODO: >1 .
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

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
							if (varName == m_Sets[setId].uniforms[index]._members[member].name)
							{
								memcpy(m_CpuBuffer + m_UniqueSize + offset + (m_NonUniqueSize * matId), value, m_Sets[setId].uniforms[index]._members[member].size);
								//std::cout << "Found Uniform : " << name.substr(0, pos) + "." << varName << std::endl;
							}
							offset += m_Sets[setId].uniforms[index]._members[member].size;
						}
						return;
					}
				}
			}
		}
	}

	void UniformsBufferManager::SetTexture(std::string name, uint32_t idMat, uint32_t textureId)
	{
		for (uint32_t setId = 0 ; setId < m_UniqueSets.size() ; setId++)
		{
			for (uint32_t index = 0 ; index < m_UniqueSets[setId].uniformsTexture.size() ; index++)
			{
				if (name == m_UniqueSets[setId].uniformsTexture[index]._name)
				{
					if (m_UniqueSets[setId].uniformsTexture[index]._textureId[0] != Soon::IdError)
						GraphicsRenderer::GetInstance()->RemoveTexture(m_UniqueSets[setId].uniformsTexture[index]._textureId[0]);
					//GraphicsRenderer::GetInstance()->AddTexture(textureId);
					m_UniqueSets[setId].uniformsTexture[index]._textureId[0] = textureId;
				}
			}
		}

		for (uint32_t setId = 0 ; setId < m_Sets.size() ; setId++)
		{
			for (uint32_t index = 0 ; index < m_Sets[setId].uniformsTexture.size() ; index++)
			{
				if (name == m_Sets[setId].uniformsTexture[index]._name)
				{
					if (m_Sets[setId].uniformsTexture[index]._textureId[idMat] != Soon::IdError)
						GraphicsRenderer::GetInstance()->RemoveTexture(m_Sets[setId].uniformsTexture[index]._textureId[idMat]);
					//TODO : GraphicsRenderer::GetInstance()->AddTexture(textureId);
					m_Sets[setId].uniformsTexture[index]._textureId[idMat] = textureId;
				}
			}
		}
	}

	void UniformsBufferManager::DestroyAllUniforms(void)
	{
		if (m_UniqueSize + (m_NonUniqueSize * m_MinElements) == 0)
			return ;

		// TODO: VkDescriptorSet
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

		for (uint32_t idChain = 0; idChain < GraphicsInstance::GetInstance()->GetSwapChainSize(); idChain++)
			vmaDestroyBuffer(GraphicsInstance::GetInstance()->GetAllocator(), m_GpuBuffer[idChain], m_GpuMemory[idChain]);
	}

	void UniformsBufferManager::UpdateToGPU( uint32_t currentImg )
	{
		if (m_UniqueSize + (m_NonUniqueSize * m_MinElements) == 0)
			return ;

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

			// ALLOC BUFFER
			// TODO: Runtime UNIQUE
			// TODO: IF RUNTIME STRUCT IN STRUCT numIn NOT UPDATE
			for (uint32_t uniRunId = 0 ; uniRunId < m_Sets[index].uniformsRuntime.size() ; uniRunId++)
			{
				if (idMat >= m_Sets[index].uniformsRuntime[uniRunId].mMembers.back().numInBuffer.size())
					m_Sets[index].uniformsRuntime[uniRunId].mMembers.back().numInBuffer.push_back(1);

				m_Sets[index].uniformsRuntime[uniRunId].mBuffers.emplace_back(VK_BUFFER_USAGE_TRANSFER_DST_BIT | 
																				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
																				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
																				VMA_MEMORY_USAGE_GPU_ONLY,
																				GetUniformRuntimeSize(m_Sets[index].uniformsRuntime[uniRunId], idMat));
			}
			//

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

			GraphicsInstance::GetInstance()->UpdateRuntimeDescriptorSets(
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
		// TODO: LIST
	}

	std::vector<VkDescriptorSetLayout> UniformsBufferManager::CreateDescriptorSetLayout( void )
	{
		/*
		for (uint32_t index = 0 ; index < uboLayoutBinding.size() ; index++)
		{
			std::cout << "Set : " << index << std::endl;
			for (uint32_t index2 = 0 ; index2 < uboLayoutBinding[index].size() ; index2++)
			{
				std::cout << "Binding : " << uboLayoutBinding[index][index2].binding << std::endl;
				std::cout << "descriptorCount : " << uboLayoutBinding[index][index2].descriptorCount << std::endl;
			}
		}
		*/
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

	const UniformRuntime& UniformsBufferManager::GetUniformRuntime(std::string name) const
	{
		if (name.empty())
			throw std::runtime_error("Name == 0");

		for (uint32_t setId = 0; setId < m_Sets.size() ; setId++)
		{
			for (uint32_t index = 0; index < m_Sets[setId].uniformsRuntime.size() ; index++)
			{
				if (m_Sets[setId].uniformsRuntime[index].mName == name)
					return m_Sets[setId].uniformsRuntime[index];
			}
		}
		for (uint32_t setId = 0; setId < m_UniqueSets.size() ; setId++)
		{
			for (uint32_t index = 0; index < m_UniqueSets[setId].uniformsRuntime.size() ; index++)
			{
				if (m_UniqueSets[setId].uniformsRuntime[index].mName == name)
				{
					return m_UniqueSets[setId].uniformsRuntime[index];
				}
			}
		}
	}

	void UniformsBufferManager::SetRuntimeVarAmount(UniformRuntime& var, std::string name, uint32_t amount, uint32_t idMat)
	{
		uint32_t offset = 0;
		void *data = nullptr;
		size_t pos = name.find(".");

		for (uint32_t index = 0 ; index < var.mMembers.size() ; index++)
		{
			if (name == var.mMembers[index]._name && var.mMembers[index].isRuntime)
			{
				uint32_t runtimeSize = GetUniformRuntimeSize(var, idMat);
				runtimeSize += var.mMembers[index]._size * (amount - var.mMembers[index].numInBuffer[idMat]);
				var.mBuffers[idMat].Resize(runtimeSize);
				var.mMembers[index].numInBuffer[idMat] = amount;
				// Update
			}
		}
		// TODO: ERROR
/*
		while (name.find(".") != std::string::npos)
		{
			if (name == uniform.mName)
			{
			}
		}
*/
	}

	// TODO: IdMat norme order
	void UniformsBufferManager::SetRuntimeAmount(std::string name, uint32_t amount, uint32_t idMat)
	{
		if (amount == 0)
			throw std::runtime_error("Amount == 0");

		size_t pos = name.find(".");
		if (pos == std::string::npos)
			return ; // TODO: ERROR
		for (uint32_t setId = 0; setId < m_Sets.size() ; setId++)
		{
			for (uint32_t index = 0; index < m_Sets[setId].uniformsRuntime.size() ; index++)
			{
				if (m_Sets[setId].uniformsRuntime[index].mName == name.substr(0, pos))
				{
					SetRuntimeVarAmount(m_Sets[setId].uniformsRuntime[index], name.substr(pos + 1), amount, idMat);
					std::vector<VkDescriptorSet> descriptorSets;
					descriptorSets.resize(m_DescriptorSets.size());
					for (uint32_t descriptorId = 0 ; descriptorId < m_DescriptorSets.size() ; descriptorId++)
						descriptorSets[descriptorId] = m_DescriptorSets[descriptorId][m_UniqueSets.size() + (m_Sets.size() * idMat) + index];
					GraphicsInstance::GetInstance()->UpdateRuntimeDescriptorSets(&idMat,
																				1,
																				m_Sets[setId],
																				descriptorSets.data());
					return;
				}
			}
		}
		for (uint32_t setId = 0; setId < m_UniqueSets.size() ; setId++)
		{
			for (uint32_t index = 0; index < m_UniqueSets[setId].uniformsRuntime.size() ; index++)
			{
				if (m_UniqueSets[setId].uniformsRuntime[index].mName == name.substr(0, pos))
				{
					SetRuntimeVarAmount(m_UniqueSets[setId].uniformsRuntime[index], name.substr(pos + 1), amount, idMat);
					std::vector<VkDescriptorSet> descriptorSets;
					descriptorSets.resize(m_DescriptorSets.size());
					for (uint32_t descriptorId = 0 ; descriptorId < m_DescriptorSets.size() ; descriptorId++)
						descriptorSets[descriptorId] = m_DescriptorSets[descriptorId][setId];
					GraphicsInstance::GetInstance()->UpdateRuntimeDescriptorSets(&idMat,
																				1,
																				m_UniqueSets[setId],
																				descriptorSets.data());
					return;
				}
			}
		}
	}

}; // namespace Soon
