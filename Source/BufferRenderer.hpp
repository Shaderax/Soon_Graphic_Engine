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
		BufferRenderer(const BufferRenderer&) = delete;
		BufferRenderer(BufferRenderer&& other);
		void Destroy( void );

		BufferRenderer& operator=(BufferRenderer&& other) noexcept;
		BufferRenderer& operator=( const BufferRenderer& other) = delete;

		const VkBuffer& GetBuffer( void ) const;
		const VmaAllocation& GetAllocation( void ) const;
		VkDeviceSize GetSize( void ) const;

		void Resize( VkDeviceSize size );

		//void Resize( uint32_t size );
	};
}