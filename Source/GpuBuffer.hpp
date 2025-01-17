#pragma once

#include "RendererRessource.hpp"

#include "vulkan/vulkan.h"
#include "VkMemoryAllocator/vk_mem_alloc.h"

namespace Soon
{
	class GpuBuffer : public RendererRessource
	{
	private:
		VkBufferUsageFlags m_BufferFlags;
		VmaMemoryUsage m_MemUsg;
		uint32_t m_Size = 0;
	protected:
	public:
		uint32_t m_Offset = 0;
		GpuBuffer( VkBufferUsageFlags flags, VmaMemoryUsage memUsg, uint32_t size );
		~GpuBuffer( void );
		GpuBuffer(const GpuBuffer& other);
		GpuBuffer(GpuBuffer&& other);
		GpuBuffer& operator=(GpuBuffer& other);
		void SetData( void* data, uint32_t size );
		void Resize( uint32_t size );
		VkBufferUsageFlags GetBufferUsage( void ) const;
		VmaMemoryUsage GetMemoryUsage( void ) const;
		uint32_t GetSize(void) const;
	};
}