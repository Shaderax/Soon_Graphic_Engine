#include "Utilities/Vertex.hpp"

#include "Math/vec3.hpp"
#include <vector>
#include <assert.h>

namespace Soon
{

std::vector<VertexInput> DefaultVertexInput{
	{"inPosition", EnumVertexElementSementic::POSITION},
	{"inNormal", EnumVertexElementSementic::NORMAL},
	{"inTexCoord", EnumVertexElementSementic::TEXCOORD}};

VertexElementType::VertexElementType(void) : baseType(EnumVertexElementType::UNKNOW), column(1), row(1)
{
}

VertexElementType::VertexElementType(EnumVertexElementType type, uint32_t pLength, uint32_t pColumn)
{
	baseType = type;
	row = pLength;
	column = pColumn;
}

uint32_t VertexElementType::GetTypeSize()
{
	switch (baseType)
	{
	case EnumVertexElementType::BOOLEAN:
		return (sizeof(bool) * row * column);
	case EnumVertexElementType::INT:
		return (sizeof(uint32_t) * row * column);
	case EnumVertexElementType::UINT:
		return (sizeof(uint32_t) * row * column);
	case EnumVertexElementType::FLOAT:
		return (sizeof(float) * row * column);
	case EnumVertexElementType::DOUBLE:
		return (sizeof(double) * row * column);
	default:
		return (0);
	}
	return (0);
}

bool operator==(const VertexElementType &lhs, const VertexElementType &rhs)
{
	return (lhs.baseType == rhs.baseType && lhs.row == rhs.row && lhs.column == rhs.column);
}

bool operator!=(const VertexElementType &lhs, const VertexElementType &rhs)
{
	return !(lhs == rhs);
}

bool VertexDescription::HasElement(EnumVertexElementSementic sem)
{
	for (VertexElement &element : data)
	{
		if (element.sementic == sem)
		{
			return (true);
		}
	}
	return (false);
}

void VertexDescription::AddVertexElement(VertexElement element)
{
	// true for nothing
	assert(!HasElement(element.sementic) && "Has Element");

	strideSize += element.type.GetTypeSize();
	data.push_back(element);
	std::cout << "ICICICICI : " << element.type.GetTypeSize() << std::endl;
}

void VertexDescription::AddVertexElement(EnumVertexElementSementic sem, EnumVertexElementType type, uint32_t column, uint32_t row)
{
	assert((!HasElement(sem) && "Has Element"));

	VertexElement ve;
	ve.sementic = sem;
	ve.type.baseType = type;
	ve.type.row = row;
	ve.type.column = column;
	data.push_back(ve);
	strideSize += ve.type.GetTypeSize();
}

void VertexDescription::RemoveVertexElement(EnumVertexElementSementic sem)
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

uint32_t VertexDescription::GetVertexStrideSize(void)
{
	return (strideSize);
}

uint32_t VertexDescription::GetElementOffset(EnumVertexElementSementic sem)
{
	assert(HasElement(sem) && "Has Element");

	uint32_t offset = 0;

	for (VertexElement &element : data)
	{
		if (sem == element.sementic)
			break;
		offset += element.type.GetTypeSize();
	}
	return (offset);
}

uint32_t VertexDescription::GetElementOffset( uint32_t id )
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

uint32_t VertexDescription::GetNumElement( void )
{
	return (data.size());
}

bool operator==(const VertexDescription &lhs, const VertexDescription &rhs)
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
bool operator!=(const VertexDescription &lhs, const VertexDescription &rhs)
{
	return !(lhs == rhs);
}
} // namespace Soon