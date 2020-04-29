#pragma once

#include "Math/vec3.hpp"
#include <vector>
#include <assert.h>

namespace Soon
{
enum class EnumVertexElementSementic : uint16_t
{
	POSITION = 0, /**< Position */
	//VES_BLEND_WEIGHTS = 2, /**< Blend weights */
	//VES_BLEND_INDICES = 3, /**< Blend indices */
	NORMAL = 1, /**< Normal */
	//VES_COLOR = 5, /**< Color */
	TEXCOORD = 2, /**< UV coordinate */
				  //VES_BITANGENT = 7, /**< Bitangent */
				  //VES_TANGENT = 8, /**< Tangent */
				  //VES_POSITIONT = 9, /**< Transformed position */
				  //VES_PSIZE = 10 /**< Point size */
};

struct VertexInput
{
	std::string inputName;
	EnumVertexElementSementic type;
};

enum class EnumVertexElementType : uint8_t
{
	UNKNOW = 0,
	VOID = 1,
	BOOLEAN = 2,
	SBYTE = 3,
	UBYTE = 4,
	SHORT = 5,
	USHORT = 6,
	INT = 7,
	UINT = 8,
	INT64 = 9,
	UINT64 = 10,
	ATOMICCOUNTER = 11,
	HALF = 12,
	FLOAT = 13,
	DOUBLE = 14,
	STRUCT = 15,
	IMAGE = 16,
	SAMPLEDIMAGE = 17,
	SAMPLER = 18,
	ACCELERATIONSTRUCTURENV = 19,

	// Keep internal types at the end.
	CONTROLPOINTARRAY = 20,
	CHAR = 21,
};

struct VertexElementType
{
	VertexElementType(void);

	VertexElementType(EnumVertexElementType type, uint32_t pLength, uint32_t pColumn);

	EnumVertexElementType baseType;
	uint32_t row;
	uint32_t column;

	uint32_t GetTypeSize();

	/*
	bool operator==(const VertexElementType &lhs, const VertexElementType &rhs)
	{
		return (lhs.baseType == rhs.baseType && lhs.row == rhs.row && lhs.column == rhs.column);
	}

	bool operator!=(const VertexElementType &lhs, const VertexElementType &rhs)
	{
		return !(lhs == rhs);
	}
	*/
};

bool operator==(const VertexElementType &lhs, const VertexElementType &rhs);
bool operator!=(const VertexElementType &lhs, const VertexElementType &rhs);

struct VertexElement
{
	VertexElement( void )
	{

	}

	VertexElement(EnumVertexElementSementic _sem, VertexElementType _type)
	{
		sementic = _sem;
		type = _type;
	}

	EnumVertexElementSementic sementic;
	VertexElementType type;
};

struct VertexDescription
{
	std::vector<VertexElement> data;
	uint32_t strideSize = 0;

	bool HasElement(EnumVertexElementSementic sem);
	void AddVertexElement(VertexElement element);
	void AddVertexElement(EnumVertexElementSementic sem, EnumVertexElementType type, uint32_t column, uint32_t row);
	void RemoveVertexElement(EnumVertexElementSementic sem);
	uint32_t GetVertexStrideSize(void);
	uint32_t GetElementOffset(EnumVertexElementSementic sem);
	uint32_t GetElementOffset( uint32_t id );
	uint32_t GetNumElement( void );
};

bool operator==(const VertexDescription &lhs, const VertexDescription &rhs);
bool operator!=(const VertexDescription &lhs, const VertexDescription &rhs);

extern std::vector<VertexInput> DefaultVertexInput;

} // namespace Soon