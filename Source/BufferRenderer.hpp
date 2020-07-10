#include "vulkan/vulkan.h"
#include "VkMemoryAllocator/vk_mem_alloc.h"

namespace Soon
{
	class BufferRenderer
	{
	private:
		VkBuffer m_Buffer;	
		VmaAllocation m_Allocation;
		VkBufferUsageFlags m_BufferFlags;
		VmaMemoryUsage m_MemUsg;
		VkDeviceSize m_Size;
	protected:
	public:
		uint32_t count = 0;
		BufferRenderer( VkBufferUsageFlags flags, VmaMemoryUsage memUsg, uint64_t size );
		~BufferRenderer( void );
		BufferRenderer(BufferRenderer&) = delete;
		BufferRenderer(BufferRenderer&&);
		void Destroy( void );

		BufferRenderer& operator=(BufferRenderer&& other) noexcept;

		const VkBuffer& GetBuffer( void ) const;
		const VmaAllocation& GetAllocation( void ) const;
		VkDeviceSize GetSize( void ) const;

		void Resize( uint32_t size );
	};
}