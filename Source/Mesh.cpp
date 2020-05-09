#include "Renderer/Mesh.hpp"

namespace Soon
{
Mesh::Mesh(VertexDescription v)
{
	_vertexDescription = v;
}

Mesh::Mesh(const Mesh &other)
{
	//_vertices = other._vertices;
	//_indices = other._indices;

	// Set Vertex Description.
}

Mesh::~Mesh(void)
{
	// if (_uniqueId != IdError)
	// 		FreeGpu();
}

// Loader Free ?

void Mesh::SetVertexElement(uint8_t *data, uint32_t size, VertexElement elem)
{
	assert((_vertexDescription.HasElement(elem.sementic) && "Has Element"));

	uint32_t elementSize = elem.type.GetTypeSize(); // element size, 12
	//std::cout << "elementSize : " << elementSize << std::endl;
	_numVertex = size;
	_vertexTotalSize = _numVertex * _vertexDescription.GetVertexStrideSize();
	//std::cout << "_numVertx : " << _numVertex << std::endl;
	_vertexData = new uint8_t[_vertexTotalSize];
	//std::cout << "Allocate Size : " << _numVertex * elem.type.row * elem.type.column << std::endl;
	uint32_t offset = _vertexDescription.GetElementOffset(elem.sementic); // offset of element in stride, Ex : 0
	//std::cout << "offset : " << offset << std::endl;
	uint32_t strideSize = _vertexDescription.GetVertexStrideSize(); // Ex : 12oct
	//std::cout << "strideSize : " << strideSize << std::endl;

	for (uint32_t index = 0; index < size; index++)
		memcpy(_vertexData + (index * strideSize) + offset, data + (index * elementSize), elementSize);
}

// Material Handle
void Mesh::SetMaterial(ShaderMaterial material)
{
	_material = material;
}

ShaderMaterial Mesh::GetMaterial()
{
	return (_material);
}

void Mesh::Render()
{
	if (!_material.HasValidVertexDescription(_vertexDescription))
	{
		std::cout << "Non Valid Mesh To Pipeline" << std::endl;
		return ;
	}
	if (_uniqueId == Soon::IdError)
		AllocGpu();
	_material.AddToPipeline(_uniqueId);
}

void Mesh::UnRender()
{
}

void Mesh::AllocGpu()
{
	_uniqueId = GraphicsRenderer::GetInstance()->AddMesh(this);
	//std::cout << "Mesh Alloc GPU, id = " << _uniqueId << std::endl;
}

void Mesh::FreeGpu()
{
	//GraphicsRenderer::GetInstance()->RemoveMesh(_uniqueId);
	_uniqueId = 0;
}

void Mesh::SetIndexBuffer( uint32_t* indexData, uint32_t size )
{
	// Need to free indexData
	_indices = new uint32_t[size];
	memcpy(_indices, indexData, size * sizeof(uint32_t));
	_numIndices = size;
}
}