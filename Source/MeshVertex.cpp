#include "MeshVertex.hpp"

#include "Math/vec3.hpp"
#include <vector>
#include <assert.h>
#include "vulkan/vulkan.h"

namespace Soon
{
bool MeshVertexDescription::HasElement(EnumVertexElementSementic sem)
{
	for (MeshVertexElement &element : data)
	{
		if (element.sementic == sem)
		{
			return (true);
		}
	}
	return (false);
}

void MeshVertexDescription::AddVertexElement(MeshVertexElement element)
{
	// true for nothing
	assert(!HasElement(element.sementic) && "Has Element");

	strideSize += element.type.GetTypeSize();
	data.push_back(element);
}

void MeshVertexDescription::AddVertexElement(EnumVertexElementSementic sem, EnumVertexElementType type, uint32_t column, uint32_t row)
{
	assert((!HasElement(sem) && "Has Element"));

	MeshVertexElement ve;
	ve.sementic = sem;
	ve.type.baseType = type;
	ve.type.row = row;
	ve.type.column = column;
	data.push_back(ve);
	strideSize += ve.type.GetTypeSize();
}

void MeshVertexDescription::RemoveVertexElement(EnumVertexElementSementic sem)
{
	assert(!HasElement(sem) && "Has Element");

	for (auto iterator = data.begin(); iterator != data.end(); iterator++)
	{
		if ((*iterator).sementic == sem)
		{
			strideSize -= (*iterator).type.GetTypeSize();
			data.erase(iterator);
		}
	}
}

uint32_t MeshVertexDescription::GetVertexStrideSize(void)
{
	return (strideSize);
}

uint32_t MeshVertexDescription::GetElementOffset(EnumVertexElementSementic sem)
{
	assert(HasElement(sem) && "Has Element");

	uint32_t offset = 0;

	for (MeshVertexElement &element : data)
	{
		if (sem == element.sementic)
			break;
		offset += element.type.GetTypeSize();
	}
	return (offset);
}

uint32_t MeshVertexDescription::GetElementOffset( uint32_t id )
{
	assert( data.size() > id && "Has Element");

	uint32_t offset = 0;

	for ( uint32_t index = 0 ; index < data.size() ; index++)
	{
		if (index == id)
			return offset;
		offset += data[index].type.GetTypeSize();
	}
	return (offset);
}

uint32_t MeshVertexDescription::GetNumElement( void )
{
	return (data.size());
}

bool operator==(const MeshVertexDescription &lhs, const MeshVertexDescription &rhs)
{
	if ((lhs.strideSize != rhs.strideSize) || (lhs.data.size() != rhs.data.size()))
		return (false);

	for (uint32_t index = 0 ; index < rhs.data.size() ; index++)
	{
		if ((lhs.data[index].sementic != rhs.data[index].sementic) || (lhs.data[index].type != rhs.data[index].type))
			return (false);
	}
	return (true);
}
bool operator!=(const MeshVertexDescription &lhs, const MeshVertexDescription &rhs)
{
	return !(lhs == rhs);
}
} // namespace Soon
