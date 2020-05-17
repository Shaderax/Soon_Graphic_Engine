#pragma once

#include "vulkan/vulkan.h"
#include "ThirdParty/VkMemoryAllocator/vk_mem_alloc.h"

namespace Soon
{
	class GpuBuffer
	{
	private:
		VkBuffer m_Buffer;
		VmaAllocation* m_Allocation;
		uint32_t m_Size;
		uint32_t m_Offset;
		VmaAllocator* m_Allocator;
		
	public:
		GpuBuffer( void )
		{

		}

		~GpuBuffer( void )
		{
			vmaDestroyBuffer(allocator, m_Buffer, m_Allocation);
		}
	};
} // namespace Soon