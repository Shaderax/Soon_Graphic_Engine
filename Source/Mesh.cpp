#include "Mesh.hpp"

#include <assert.h>

namespace Soon
{
Mesh::Mesh( VertexDescription v )
{
	m_VertexDescription = v;
}

Mesh::Mesh( const Mesh &other )
{
	//_vertices = other._vertices;
	//_indices = other._indices;
	// Set Vertex Description.
}

Mesh::~Mesh( void )
{
	// if (_uniqueId != IdError)
	// 		FreeGpu();
}

// Loader Free ?

void Mesh::SetVertexElement( uint8_t *data, uint32_t size, VertexElement elem )
{
	assert((m_VertexDescription.HasElement(elem.sementic) && "Has Element"));

	uint32_t elementSize = elem.type.GetTypeSize(); // element size, 12
	//std::cout << "elementSize : " << elementSize << std::endl;
	mNumVertex = size;
	mVertexTotalSize = mNumVertex * m_VertexDescription.GetVertexStrideSize();
	//std::cout << "_numVertx : " << _numVertex << std::endl;
	mVertexData = new uint8_t[mVertexTotalSize];
	//std::cout << "Allocate Size : " << _numVertex * elem.type.row * elem.type.column << std::endl;
	uint32_t offset = m_VertexDescription.GetElementOffset(elem.sementic); // offset of element in stride, Ex : 0
	//std::cout << "offset : " << offset << std::endl;
	uint32_t strideSize = m_VertexDescription.GetVertexStrideSize(); // Ex : 12oct
	//std::cout << "strideSize : " << strideSize << std::endl;

	for (uint32_t index = 0; index < size; index++)
		memcpy(mVertexData + (index * strideSize) + offset, data + (index * elementSize), elementSize);
}

// Material Handle
void Mesh::SetMaterial(ShaderMaterial material)
{
	m_Material = material;
}

ShaderMaterial Mesh::GetMaterial()
{
	return (m_Material);
}

void Mesh::Render()
{
	if (!m_Material.HasValidVertexDescription(m_VertexDescription))
	{
		std::cout << "Non Valid Mesh To Pipeline" << std::endl;
		return ;
	}
	if (m_UniqueId == Soon::IdError)
		AllocGpu();
	m_Material.AddToPipeline(m_UniqueId);
}

void Mesh::UnRender()
{
}

void Mesh::AllocGpu()
{
	m_UniqueId = GraphicsRenderer::GetInstance()->AddMesh(this);
	//std::cout << "Mesh Alloc GPU, id = " << _uniqueId << std::endl;
}

void Mesh::FreeGpu()
{
	//GraphicsRenderer::GetInstance()->RemoveMesh(_uniqueId);
	m_UniqueId = 0;
}

void Mesh::SetIndexBuffer( uint32_t* indexData, uint32_t size )
{
	// Need to free indexData
	mIndices = new uint32_t[size];
	memcpy(mIndices, indexData, size * sizeof(uint32_t));
	mNumIndices = size;
}
}