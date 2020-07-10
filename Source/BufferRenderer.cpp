#include "BufferRenderer.hpp"

#include "GraphicsInstance.hpp"

namespace Soon
{
	BufferRenderer::BufferRenderer( VkBufferUsageFlags flags, VmaMemoryUsage memUsg, uint64_t size ) : m_BufferFlags(flags), m_MemUsg(memUsg), m_Size(size)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = flags;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = memUsg;

		vmaCreateBuffer(GraphicsInstance::GetInstance()->GetAllocator(), &bufferInfo, &allocInfo, &m_Buffer, &m_Allocation, nullptr);
	}

	BufferRenderer::~BufferRenderer( void )
	{
	}

	void BufferRenderer::Destroy( void )
	{
		if (m_Buffer != VK_NULL_HANDLE)
			vmaDestroyBuffer(GraphicsInstance::GetInstance()->GetAllocator(), m_Buffer, m_Allocation);
	}

	BufferRenderer::BufferRenderer(BufferRenderer&& other)
	{
		m_Buffer = other.m_Buffer;
		other.m_Buffer = VK_NULL_HANDLE;
		m_Allocation = other.m_Allocation;
		other.m_Allocation = VK_NULL_HANDLE;
		m_Size = std::exchange(other.m_Size, 0);
	}

	void BufferRenderer::Resize( uint32_t size )
	{

	}

	const VkBuffer& BufferRenderer::GetBuffer( void ) const
	{
		return (m_Buffer);
	}

	const VmaAllocation& BufferRenderer::GetAllocation( void ) const
	{
		return (m_Allocation);
	}

	VkDeviceSize BufferRenderer::GetSize( void ) const
	{
		return (m_Size);
	}

	BufferRenderer& BufferRenderer::operator=(BufferRenderer&& other) noexcept
	{
    	if(this != &other)
		{
			std::cout << "BEFORE THE STORM" << std::endl;
			m_Buffer = other.m_Buffer;
			other.m_Buffer = VK_NULL_HANDLE;
			m_Allocation = other.m_Allocation;
			other.m_Allocation = VK_NULL_HANDLE;
			m_Size = std::exchange(other.m_Size, 0);
    	}
    	return *this;
	}
}