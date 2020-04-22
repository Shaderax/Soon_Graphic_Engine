#pragma once

#include <vector>
#include <string>
#include "Utilities/Vertex.hpp"
#include <limits>

namespace Soon
{
	class Mesh
	{
		private:
			VertexDescription _vertexDescription;
			uint8_t* _vertexData;
			uint8_t* _indices;
			uint32_t _numIndices;
			uint32_t _numVertex;

			uint32_t uniqueId = std::numeric_limits<uint32_t>::max();

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
			}

			// Loader Free ?

			void SetVertexElement(uint8_t* data, uint32_t size, VertexElement elem)
			{
				assert(!_vertexDescription.HasElement(elem.sementic) && "Has Element");

				uint32_t elementSize = elem.type.GetTypeSize();
				uint32_t offset = _vertexDescription.GetElementOffset(elem.sementic);
				uint32_t strideSize = _vertexDescription.GetVertexStrideSize();

				for (uint32_t index = 0; index < _numVertex; index++)
					memcpy(_vertexData + (index * strideSize) + offset, data + (index * elementSize), elementSize);
			}
	};
}; // namespace Soon
