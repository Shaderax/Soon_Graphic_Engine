#pragma once

#include "Math/vec3.hpp"
#include <vector>
#include <assert.h>
#include "Modules/SPIRV-Reflect/spirv_reflect.h"
#include "vulkan/vulkan.h"

namespace Soon
{
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
	};

	bool operator==(const VertexElementType &lhs, const VertexElementType &rhs);
	bool operator!=(const VertexElementType &lhs, const VertexElementType &rhs);

	struct VertexElement
	{
		VertexElement(void)
		{
		}

		VertexElement(VertexElementType _type, uint32_t offset)
		{
			type = _type;
			mOffset = offset;
		}

		VertexElementType type;
		uint32_t mOffset = 0;
	};

	struct VertexDescription
	{
		std::vector<VertexElement> data;
		uint32_t strideSize = 0;

		void AddVertexElement(VertexElement element);
		void AddVertexElement(EnumVertexElementType type, uint32_t column, uint32_t row);
		void RemoveVertexElement(uint32_t id);
		uint32_t GetVertexStrideSize(void) const;
		uint32_t GetElementOffset(uint32_t id);
		uint32_t GetNumElement(void);
	};

	bool operator==(const VertexDescription &lhs, const VertexDescription &rhs);
	bool operator!=(const VertexDescription &lhs, const VertexDescription &rhs);

	VertexElementType SpvTypeToVertexType(SpvReflectTypeDescription *type);
	bool IsImageType(SpvReflectTypeFlags type);

	struct DefaultInputBinding
	{
		uint32_t mBinding;
		VkVertexInputRate mInputRate;
		std::vector<std::string> mInputName;
	};

	struct InputBindingDescription
	{
		uint32_t mBinding;
		VkVertexInputRate mInputRate;
		VertexDescription mDescription;
	};

} // namespace Soon