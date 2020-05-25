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
		VertexElementType _type;
		uint32_t _size;
		uint32_t _binding;
		uint32_t _set;
		std::vector<UniformVar> _members;
		std::function<void(int)> _updateFunct;
	};

	struct UniformTexture
	{
		std::string _name;
		VertexElementType _type;
		uint32_t _binding;
		uint32_t _set;
		std::vector<uint32_t> _textureId;	// TODO: For uniqueTexture change vector
		std::function<void(int)> _updateFunct;
	};

	class UniformsBufferManager
	{
	private:
		uint32_t		m_NumElements = 0;
		uint32_t		m_MinElements = 100;
		uint32_t		m_ActualResize = 0;
		uint32_t		m_SizeUniqueUniformBlock = 0;
		uint32_t		m_SizeUniformBlock = 0;

		uint8_t*	m_CpuBuffer = nullptr;
		std::vector<VmaAllocation>	m_GpuMemory;
		std::vector<VkBuffer>		m_GpuBuffer; // TODO: Aligment in vulkan (16)

		std::vector<Uniform>		m_Uniforms;
		std::vector<Uniform>		m_UniqueUniforms;

		std::vector<UniformTexture>		m_UniformsTexture;
		std::vector<UniformTexture>		m_UniqueUniformsTexture;

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
		void AddUniqueUniform( Uniform uniform );
		void AddUniqueUniform( UniformTexture uniform );
		void UpdateToGPU( uint32_t currentImg );
		void Set( std::string name, void* value, uint32_t matId );
		void SetTexture(std::string name, uint32_t idMat, uint32_t textureId);
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