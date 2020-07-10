#include "Vertex.hpp"

#include "Math/vec3.hpp"
#include <vector>
#include <assert.h>
#include "vulkan/vulkan.h"

namespace Soon
{
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

void VertexDescription::AddVertexElement(VertexElement element)
{
	strideSize += element.type.GetTypeSize();
	data.push_back(element);
}

void VertexDescription::AddVertexElement(EnumVertexElementType type, uint32_t column, uint32_t row)
{
	VertexElement ve;
	ve.type.baseType = type;
	ve.type.row = row;
	ve.type.column = column;
	data.push_back(ve);
	strideSize += ve.type.GetTypeSize();
}

void VertexDescription::RemoveVertexElement(uint32_t id)
{
	if (id >= data.size())
		throw std::runtime_error("Id >= data.size()");

	strideSize -= data[id].type.GetTypeSize();
	data.erase(data.begin() + id);
}

uint32_t VertexDescription::GetVertexStrideSize(void) const
{
	return (strideSize);
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
		if ((lhs.data[index].type != rhs.data[index].type) || (lhs.data[index].mOffset != rhs.data[index].mOffset))
			return (false);
	}
	return (true);
}
bool operator!=(const VertexDescription &lhs, const VertexDescription &rhs)
{
	return !(lhs == rhs);
}

	VertexElementType SpvTypeToVertexType(SpvReflectTypeDescription* type)
	{
		VertexElementType ve;

		if (type->type_flags & SPV_REFLECT_TYPE_FLAG_VOID)
			ve.baseType = EnumVertexElementType::VOID;
		else if (type->type_flags & SPV_REFLECT_TYPE_FLAG_BOOL)
			ve.baseType = EnumVertexElementType::BOOLEAN;
		else if (type->type_flags & SPV_REFLECT_TYPE_FLAG_INT)
		{
			if (type->traits.numeric.scalar.signedness == 1)
				ve.baseType = EnumVertexElementType::INT;
			else
				ve.baseType = EnumVertexElementType::UINT;
		}
		else if (type->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT)
		{
			if (type->traits.numeric.scalar.width == 32)
				ve.baseType = EnumVertexElementType::FLOAT;
			else
				ve.baseType = EnumVertexElementType::DOUBLE;
		}
		else if (type->type_flags & SPV_REFLECT_TYPE_FLAG_EXTERNAL_IMAGE)
			ve.baseType = EnumVertexElementType::IMAGE;
		else if (type->type_flags & SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLER)
			ve.baseType = EnumVertexElementType::SAMPLER;
		else if (type->type_flags & SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLED_IMAGE)
			ve.baseType = EnumVertexElementType::SAMPLEDIMAGE;
		else if (type->type_flags & SPV_REFLECT_TYPE_FLAG_STRUCT)
			ve.baseType = EnumVertexElementType::STRUCT;

		switch (type->op)
		{
		case (SpvOpTypeVector):
			ve.row = 1;
			ve.column = type->traits.numeric.vector.component_count;
			break;
		case (SpvOpTypeMatrix):
			ve.row = type->traits.numeric.matrix.row_count;
			ve.column = type->traits.numeric.matrix.column_count;
			break;
		default:
			ve.row = 1;
			ve.column = 1;
			break;
		}
		return (ve);
	}

	VkDescriptorType DescriptorTypeSpvToVk(SpvReflectDescriptorType type)
	{
		switch (type)
		{
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
			return (VK_DESCRIPTOR_TYPE_SAMPLER);
		case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			return (VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
			return (VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
			return (VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
			return (VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER);
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
			return (VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER);
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			return (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
			return (VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
			return (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
		case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			return (VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC);
		case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
			return (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);
		default:
			return (VkDescriptorType::VK_DESCRIPTOR_TYPE_MAX_ENUM);
		}
		return (VkDescriptorType::VK_DESCRIPTOR_TYPE_MAX_ENUM);
	}

	VkShaderStageFlags SpvStageToVulkanStage(SpvReflectShaderStageFlagBits stage)
	{
		switch (stage)
		{
		case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
			return (VK_SHADER_STAGE_VERTEX_BIT);
		case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
			return (VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
		case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
			return (VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
		case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
			return (VK_SHADER_STAGE_GEOMETRY_BIT);
		case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
			return (VK_SHADER_STAGE_FRAGMENT_BIT);
		case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
			return (VK_SHADER_STAGE_COMPUTE_BIT);
		default:
			break;
		}
		return (0);
	}
	VkFormat VertexTypeToVkFormat(VertexElementType vt)
	{
		switch (vt.baseType)
		{
		case EnumVertexElementType::INT:
			switch (vt.column)
			{
			case 1:
				return (VK_FORMAT_R32_SINT);
			case 2:
				return (VK_FORMAT_R32G32_SINT);
			case 3:
				return (VK_FORMAT_R32G32B32_SINT);
			case 4:
				return (VK_FORMAT_R32G32B32A32_SINT);
			default:
				return (VK_FORMAT_UNDEFINED);
			}
		case EnumVertexElementType::UINT:
			switch (vt.column)
			{
			case 1:
				return (VK_FORMAT_R32_UINT);
			case 2:
				return (VK_FORMAT_R32G32_UINT);
			case 3:
				return (VK_FORMAT_R32G32B32_UINT);
			case 4:
				return (VK_FORMAT_R32G32B32A32_UINT);
			default:
				return (VK_FORMAT_UNDEFINED);
			}
		case EnumVertexElementType::FLOAT:
			switch (vt.column)
			{
			case 1:
				return (VK_FORMAT_R32_SFLOAT);
			case 2:
				return (VK_FORMAT_R32G32_SFLOAT);
			case 3:
				return (VK_FORMAT_R32G32B32_SFLOAT);
			case 4:
				return (VK_FORMAT_R32G32B32A32_SFLOAT);
			default:
				return (VK_FORMAT_UNDEFINED);
			}
		case EnumVertexElementType::DOUBLE:
			switch (vt.column)
			{
			case 1:
				return (VK_FORMAT_R64_SFLOAT);
			case 2:
				return (VK_FORMAT_R64G64_SFLOAT);
			case 3:
				return (VK_FORMAT_R64G64B64_SFLOAT);
			case 4:
				return (VK_FORMAT_R64G64B64A64_SFLOAT);
			default:
				return (VK_FORMAT_UNDEFINED);
			}
		default:
			return (VK_FORMAT_UNDEFINED);
		}
		return (VK_FORMAT_UNDEFINED);
	}

	bool IsImageType(SpvReflectTypeFlags type)
	{
		return ((type & SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLER || type & SPV_REFLECT_TYPE_FLAG_EXTERNAL_IMAGE || type & SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLED_IMAGE));
	}

} // namespace Soon
