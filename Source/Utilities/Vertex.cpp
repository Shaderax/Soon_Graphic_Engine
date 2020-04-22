#include "Utilities/Vertex.hpp"

#include "Math/vec3.hpp"
#include <vector>
#include <assert.h>

namespace Soon
{

std::vector<VertexInput> DefaultVertexInput{
	{"inPosition", VertexElementSementic::POSITION},
	{"inNormal", VertexElementSementic::NORMAL},
	{"inTexCoord", VertexElementSementic::TEXCOORD}};

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

bool VertexDescription::HasElement(VertexElementSementic sem)
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
	assert((!HasElement(element.sementic) && "Has Element"));

	data.push_back(element);
}

void VertexDescription::AddVertexElement(VertexElementSementic sem, EnumVertexElementType type, uint32_t column, uint32_t row)
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

void VertexDescription::RemoveVertexElement(VertexElementSementic sem)
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

uint32_t VertexDescription::GetElementOffset(VertexElementSementic sem)
{
	assert(!HasElement(sem) && "Has Element");

	uint32_t offset = 0;

	for (VertexElement &element : data)
	{
		if (sem == element.sementic)
			break;
		offset += element.type.GetTypeSize();
	}
	return (offset);
}
} // namespace Soon