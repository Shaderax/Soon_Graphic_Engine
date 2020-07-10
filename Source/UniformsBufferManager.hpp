#pragma once

#include <stdint.h>
#include <iostream>
#include "vulkan/vulkan.h"

#include <vector>
#include <functional>

#include "VkMemoryAllocator/vk_mem_alloc.h"

#include "GraphicsInstance.hpp"

namespace Soon
{
	struct IdRender;

	class UniformsBufferManager
	{
	private:
		uint32_t		m_NumElements = 0;
		uint32_t		m_MinElements = 10;
		uint32_t		m_ActualResize = 0;

		uint8_t*	m_CpuBuffer = nullptr;
		std::vector<VmaAllocation>	m_GpuMemory;
		std::vector<VkBuffer>		m_GpuBuffer; // TODO: Aligment in vulkan (32)

		uint32_t m_UniqueSize = 0;
		uint32_t m_NonUniqueSize = 0;
		std::vector<DescriptorSetDescription> m_Sets;
		std::vector<DescriptorSetDescription> m_UniqueSets;

		std::vector<std::vector<VkDescriptorSet>> m_DescriptorSets;

		std::vector<VkDescriptorSetLayout> _descriptorSetLayout;
		std::vector<std::vector<VkDescriptorSetLayoutBinding>> uboLayoutBinding;

	public:
		UniformsBufferManager( void );
		~UniformsBufferManager( void );

		void InitBuffers( void );
		void DestroyAllUniforms( void );

		void AddUniform( Uniform uniform );
		void AddUniform( UniformTexture uniform );
		void AddUniform( UniformRuntime uniform );
		void AddUniqueUniform( Uniform uniform );
		void AddUniqueUniform( UniformTexture uniform );
		void AddUniqueUniform( UniformRuntime uniform );

		void UpdateToGPU( uint32_t currentImg );
		void* Get( std::string name, uint32_t matId );
		void Set( std::string name, void* value, uint32_t matId );
		void SetTexture(std::string name, uint32_t idMat, uint32_t textureId);
		void Allocate( uint32_t idMat );
		void AddLayoutBinding(VkDescriptorSetLayoutBinding ubo, uint32_t set);
		std::vector<VkDescriptorSetLayout> CreateDescriptorSetLayout( void );
		std::vector<VkDescriptorSetLayout> GetDescriptorSetLayout( void );

		std::vector<DescriptorSetDescription>& GetSets( void );
		std::vector<DescriptorSetDescription>& GetUniqueSets( void );

		std::vector<VkDescriptorSet>& GetDescriptorSet( uint32_t image );
		void Free( uint32_t idMat );
		void RecreateUniforms( std::unordered_map<uint32_t, uint32_t>& toDraw );
		void CheckAndResize( uint32_t num );

		void SetRuntimeAmount(std::string name, uint32_t amount, uint32_t idMat);
		void SetRuntimeVarAmount(UniformRuntime& var, std::string name, uint32_t amount, uint32_t idMat);
	};
}