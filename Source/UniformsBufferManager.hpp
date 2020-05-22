#pragma once

#include <stdint.h>
#include <iostream>
#include "vulkan/vulkan.h"

#include <vector>
#include <functional>

#include "Vertex.hpp"

#include "VkMemoryAllocator/vk_mem_alloc.h"

namespace Soon
{
	struct IdRender;

	struct UniformVar
	{
		std::string _name;
		VertexElementType _type;
		uint32_t _size;
		uint32_t _offset;
	};

	struct Uniform
	{
		std::string _name;
		uint32_t _size;
		uint32_t _binding;
		uint32_t _set;
		std::vector<UniformVar> _members;
		//std::vector<UniformSets> _us;
		// Ou que je met les DescriptorSet ?
		std::function<void(int)> _updateFunct;
		bool isUnique;
	};

	struct UniformTexture
	{
		UniformVar _data;
		uint32_t _binding;
		uint32_t _set;
		//std::vector<Image> _i;
		//std::vector<ImageRenderer> _iR;
		std::vector<VkDescriptorSet> _dS;
		std::function<void(int)> _updateFunct;
		bool isUnique;
	};

	class UniformsBufferManager
	{
	private:
		uint32_t		m_NumElements = 0;
		uint32_t		m_MinElements = 100;
		uint32_t		m_ActualResize = 0;
		uint8_t*		m_CpuBuffer = nullptr;
		uint32_t		m_SizeUniqueUniformBlock = 0;
		uint32_t		m_SizeUniformBlock = 0;
		std::vector<VmaAllocation>	m_GpuMemory;
		std::vector<VkBuffer>		m_GpuBuffer; // TODO: Aligment in vulkan (16)
		std::vector<Uniform>		m_Uniforms;
		std::vector<Uniform>		m_UniqueUniforms;
		std::vector<UniformTexture> m_UniformsTexture;
		std::vector<std::vector<VkDescriptorSet>> m_DescriptorSets;

		std::vector<VkDescriptorSetLayout> _descriptorSetLayout; // TODO: UniformTexture
		std::vector<std::vector<VkDescriptorSetLayoutBinding>> uboLayoutBinding;

	public:
		UniformsBufferManager( void );
		~UniformsBufferManager( void );
		void InitBuffers( void );
		void DestroyAllUniforms( void );
		void AddUniform( Uniform uniform );
		void AddUniformTexture( UniformTexture uniform );
		void UpdateToGPU( uint32_t currentImg );
		void Set( std::string name, void* value, uint32_t matId );
		void Allocate( uint32_t idMat );
		void AddLayoutBinding(VkDescriptorSetLayoutBinding ubo, uint32_t set);
		std::vector<VkDescriptorSetLayout> CreateDescriptorSetLayout( void );
		std::vector<VkDescriptorSetLayout> GetDescriptorSetLayout( void );
		std::vector<Uniform>& GetNonUniqueUniforms( void );
		std::vector<VkDescriptorSet>& GetDescriptorSet( uint32_t image );
		void Free( uint32_t idMat );
		void RecreateUniforms( std::unordered_map<uint32_t, uint32_t>& toDraw );
		void CheckAndResize( uint32_t num );

	};
}