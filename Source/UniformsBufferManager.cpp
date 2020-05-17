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

	void UniformsBufferManager::RecreateUniforms( std::vector<Soon::IdRender> toDraw )
	{
		// Buffer Creation
		VkBufferCreateInfo bufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = m_SizeUniqueUniformBlock + (m_SizeUniformBlock * m_MinElements);
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		//
		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
 
		for ( uint32_t index = 0 ; index < GraphicsInstance::GetInstance()->GetSwapChainSize() ; index++)
			vmaCreateBuffer(GraphicsInstance::GetInstance()->GetAllocator(), &bufferInfo, &allocInfo, &m_GpuBuffer[index], &(m_GpuMemory[index]), nullptr);
		//////////////////
		uint32_t offset = 0;
		for (uint32_t index = 0; index < m_Uniforms.size(); index++)
		{
			for ( uint32_t elemId = 0 ; elemId < toDraw.size() ; elemId++ )
			{
				std::vector<VkDescriptorSet> descriptorSets;
				descriptorSets = GraphicsInstance::GetInstance()->CreateDescriptorSets(m_Uniforms[index]._size, m_Uniforms[index]._binding, offset + m_SizeUniqueUniformBlock + (m_SizeUniformBlock * toDraw[elemId].matId), _descriptorSetLayout[m_Uniforms[index]._set], m_GpuBuffer.data(), 0);				
				for ( uint32_t descriptorId = 0 ; descriptorId < descriptorSets.size() ; descriptorId++ )
				{
					// TODO: UniformTexture
					m_DescriptorSets[descriptorId][m_Uniforms.size() * toDraw[elemId].matId + index] = descriptorSets[descriptorId];
				}
				offset += m_Uniforms[index]._size;
			}
		}
	}

	void UniformsBufferManager::AddUniform(Uniform uniform)
	{
		if (m_CpuBuffer != nullptr)
			; // TODO: Error
		if (uniform.isUnique)
		{
			m_SizeUniqueUniformBlock += uniform._size;
			m_UniqueUniforms.push_back(uniform);
		}
		else
		{
			m_SizeUniformBlock += uniform._size;
			m_Uniforms.push_back(uniform);
		}
	}

	void UniformsBufferManager::AddUniformTexture( UniformTexture uniform )
	{
		/*
		if (uniform.isUnique)
		{
			m_SizeUniqueUniformBlock += uniform._size;
			m_UniqueUniforms.push_back(uniform);
		}
		else
		{
			m_SizeUniformBlock += uniform._size;
			m_Uniforms.push_back(uniform);
		}
		*/
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
			descriptor.resize((m_Uniforms.size() * m_MinElements));
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

	void UniformsBufferManager::Allocate( uint32_t idMat )
	{
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

	void UniformsBufferManager::DestroyElement( uint32_t idMat )
	{
		
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