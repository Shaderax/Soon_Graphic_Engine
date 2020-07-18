#pragma once

#include "Vertex.hpp"

#include "Math/vec3.hpp"
#include <vector>
#include <assert.h>
#include "Modules/SPIRV-Reflect/spirv_reflect.h"

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

	struct MeshVertexElement : VertexElement
	{
		MeshVertexElement(void)
		{
		}

		MeshVertexElement(EnumVertexElementSementic _sem, VertexElementType _type)
		{
			sementic = _sem;
			type = _type;
		}

		EnumVertexElementSementic sementic;
	};

	struct MeshVertexDescription
	{
		std::vector<MeshVertexElement> data;
		uint32_t strideSize = 0;

		bool HasElement(EnumVertexElementSementic sem);
		void AddVertexElement(MeshVertexElement element);
		void AddVertexElement(EnumVertexElementSementic sem, EnumVertexElementType type, uint32_t column, uint32_t row);
		void RemoveVertexElement(EnumVertexElementSementic sem);
		uint32_t GetVertexStrideSize(void);
		uint32_t GetElementOffset(EnumVertexElementSementic sem);
		uint32_t GetElementOffset(uint32_t id);
		uint32_t GetNumElement(void);
	};

	bool operator==(const MeshVertexDescription &lhs, const MeshVertexDescription &rhs);
	bool operator!=(const MeshVertexDescription &lhs, const MeshVertexDescription &rhs);

	struct DefaultMeshVertexInput
	{
		std::string inputName;
		EnumVertexElementSementic sementic;
	};

} // namespace Soon