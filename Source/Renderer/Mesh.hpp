#pragma once

#include <vector>
#include <string>
#include "Utilities/Vertex.hpp"
#include <limits>
#include <cstring>

#include <Materials/ShaderMaterial.hpp>

#include "Utilities/Error.hpp"

namespace Soon
{
	class Mesh
	{
		private:
			ShaderMaterial _material;
			VertexDescription _vertexDescription;
			uint8_t* _vertexData;
			uint8_t* _indices;
			uint32_t _numIndices;
			uint32_t _numVertex;

			uint32_t _uniqueId = std::numeric_limits<uint32_t>::max();

		public:
			Mesh(VertexDescription v)
			{
				_vertexDescription = v;
			}

			Mesh(const Mesh &other)
			{
				//_vertices = other._vertices;
				//_indices = other._indices;

				// Set Vertex Description.
			}

			~Mesh(void)
			{
				// if (_uniqueId != IdError)
				// 		FreeGpu();
			}

			// Loader Free ?

			void SetVertexElement(uint8_t* data, uint32_t size, VertexElement elem)
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
			void SetMaterial(ShaderMaterial material)
			{
				_material = material;
			}

			ShaderMaterial GetMaterial()
			{
				return (_material);
			}

			void Render()
			{
				if (_uniqueId == Soon::IdError)
					_uniqueId = AllocGpu();
				_material.AddToPipeline(_uniqueId);
			}

			void UnRender()
			{

			}

			std::uint32_t AllocGpu()
			{
				return 0;
			}

			void FreeGpu()
			{
			}
	};
}; // namespace Soon
