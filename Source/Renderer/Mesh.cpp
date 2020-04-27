#include "Utilities/Error.hpp"
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
	std::cout << "elementSize : " << elementSize << std::endl;
	_numVertex = size * elementSize;
	std::cout << "_numVertx : " << _numVertex << std::endl;
	_vertexData = new uint8_t[_numVertex];
	uint32_t offset = _vertexDescription.GetElementOffset(elem.sementic); // offset of element in stride, Ex : 0
	std::cout << "offset : " << offset << std::endl;
	uint32_t strideSize = _vertexDescription.GetVertexStrideSize(); // Ex : 12oct
	std::cout << "strideSize : " << strideSize << std::endl;

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
	if (_uniqueId == Soon::IdError)
		_uniqueId = AllocGpu();
	_material.AddToPipeline(_uniqueId);
}

void Mesh::UnRender()
{
}

std::uint32_t Mesh::AllocGpu()
{
	return 0;
}

void Mesh::FreeGpu()
{
}
}