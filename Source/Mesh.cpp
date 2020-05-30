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

}

Mesh::Mesh( const Mesh&& other )
{
	//_vertices = other._vertices;
	//_indices = other._indices;
	// Set Vertex Description.
}

Mesh::~Mesh( void )
{
	if (m_UniqueId != IdError)
		FreeGpu();
}

// Loader Free ?

void Mesh::SetVertexElement( uint8_t *data, uint32_t size, VertexElement elem )
{
	assert((m_VertexDescription.HasElement(elem.sementic) && "Has Element"));

	if (!mVertexData)
	{
		mNumVertex = size;
		mVertexTotalSize = mNumVertex * m_VertexDescription.GetVertexStrideSize();
		mVertexData = new uint8_t[mVertexTotalSize]();
	}
	std::cout << std::endl << "Elem : " << (int)elem.sementic << std::endl;
	uint32_t elementSize = elem.type.GetTypeSize(); // element size, 12
	std::cout << "elementSize : " << elementSize << std::endl;
	std::cout << "_numVertx : " << mNumVertex << std::endl;
	std::cout << "Allocate Size : " << mNumVertex * elem.type.row * elem.type.column << std::endl;
	uint32_t offset = m_VertexDescription.GetElementOffset(elem.sementic); // offset of element in stride, Ex : 0
	std::cout << "offset : " << offset << std::endl;
	uint32_t strideSize = m_VertexDescription.GetVertexStrideSize(); // Ex : 12oct
	std::cout << "strideSize : " << strideSize << std::endl;

	for (uint32_t index = 0; index < size; index++)
		memcpy(mVertexData + (index * strideSize) + offset, data + (index * elementSize), elementSize);
}

// Material Handle
void Mesh::SetMaterial(ShaderMaterial& material)
{
	m_Material = material;
}

ShaderMaterial& Mesh::GetMaterial()
{
	return (m_Material);
}

void Mesh::Render( void )
{
	if (!m_Material.HasValidVertexDescription(m_VertexDescription))
	{
		std::cout << "Non Valid Mesh To Pipeline" << std::endl;
		return ;
		// TODO: ERROR ?
	}
	if (m_UniqueId == Soon::IdError)
		AllocGpu();
	m_Material.Render(m_UniqueId);
}

void Mesh::UnRender( void )
{
	m_Material.UnRender();
}

void Mesh::AllocGpu( void )
{
	m_UniqueId = GraphicsRenderer::GetInstance()->AddMesh(this, m_UniqueId);
}

void Mesh::FreeGpu( void )
{
	if (GraphicsRenderer::_instance != nullptr)
		GraphicsRenderer::GetInstance()->RemoveMesh(m_UniqueId);
	m_UniqueId = Soon::IdError;
}

void Mesh::SetIndexBuffer( uint32_t* indexData, uint32_t size )
{
	// Need to free indexData
	mIndices = new uint32_t[size];
	memcpy(mIndices, indexData, size * sizeof(uint32_t));
	mNumIndices = size;
}
}