#include "GpuBuffer.hpp"
#include "GraphicsRenderer.hpp"

namespace Soon
{
	GpuBuffer::GpuBuffer( VkBufferUsageFlags flags, VmaMemoryUsage memUsg, uint32_t size ) : m_BufferFlags(flags),
																					m_MemUsg(memUsg),
																					m_Size(size),
																					RendererRessource(ERendererRessource::BUFFER, Soon::IdError)
	{
		if (size == 0)
			throw std::runtime_error("GpuBuffer::GpuBuffer: Size == 0");

		m_UniqueId = GraphicsRenderer::GetInstance()->AddBuffer(*this);
	}

	GpuBuffer::~GpuBuffer( void )
	{
	}

	GpuBuffer::GpuBuffer(const GpuBuffer& other) : RendererRessource(ERendererRessource::BUFFER, other.m_UniqueId)
	{
		std::cout << "Copy COnstructor" << std::endl;
		m_BufferFlags = other.GetBufferUsage();
		m_MemUsg = other.GetMemoryUsage();
		m_Size = other.GetSize();
	}

	GpuBuffer::GpuBuffer(GpuBuffer&& other) : RendererRessource(ERendererRessource::BUFFER, Soon::IdError)
	{
		std::cout << "Move COnstructor" << std::endl;
		m_BufferFlags = other.GetBufferUsage();
		m_MemUsg = other.GetMemoryUsage();
		m_Size = other.GetSize();
		m_UniqueId = other.GetId();

		other.m_UniqueId = Soon::IdError;
		other.m_Size = 0;
	}

	void GpuBuffer::Resize( uint32_t size )
	{
		BufferRenderer& buffer = GraphicsRenderer::GetInstance()->GetBufferRenderer(m_UniqueId);
		vmaResizeAllocation(GraphicsInstance::GetInstance()->GetAllocator(), buffer.GetAllocation(), size);
	}

	void GpuBuffer::SetData( void* ptrData, uint32_t size )
	{
		/*
		if (ptrData == nullptr || size == 0)
			throw std::runtime_error("GpuBuffer::SetData: Data == NULL Or Size == 0");
		
		if (size != m_Size)
			GraphicsRenderer::GetInstance()->RemoveBuffer(m_UniqueId);

		BufferRenderer buffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, size);

		if (ptrData)
		{
			void* data;
			vmaMapMemory(GraphicsInstance::GetInstance()->GetAllocator(), buffer.GetAllocation(), &data);
			memcpy(data, ptrData, (size_t)size);
			vmaUnmapMemory(GraphicsInstance::GetInstance()->GetAllocator(), buffer.GetAllocation());
		}

		BufferRenderer bufRenderer(m_BufferFlags, m_MemUsg, size);

		GraphicsInstance::GetInstance()->CopyBuffer(buffer.GetBuffer(), bufRenderer.GetBuffer(), size);

		m_UniqueId = GraphicsRenderer::GetInstance()->AddBuffer(*this, m_UniqueId);
		*/
	}

	VkBufferUsageFlags GpuBuffer::GetBufferUsage( void ) const
	{
		return m_BufferFlags;
	}

	VmaMemoryUsage GpuBuffer::GetMemoryUsage( void ) const
	{
		return m_MemUsg;
	}

	uint32_t GpuBuffer::GetSize(void) const
	{
		return m_Size;
	}
}