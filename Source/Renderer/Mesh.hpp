#pragma once

#include "Utilities/Vertex.hpp"
#include <stdint.h>
#include <limits>

#include <Materials/ShaderMaterial.hpp>

#include "Utilities/Error.hpp"

namespace Soon
{
	class Mesh
	{
		private:
			ShaderMaterial _material;
			VertexDescription _vertexDescription;

			uint32_t _uniqueId = IdError;

		public:
			uint8_t* _vertexData;
			uint32_t* _indices;
			uint32_t _numIndices;
			uint32_t _numVertex;
			uint32_t _vertexTotalSize = 0;

			Mesh(VertexDescription v);
			Mesh(const Mesh &other);
			~Mesh(void);
			// Loader Free ?

			void SetVertexElement(uint8_t* data, uint32_t size, VertexElement elem);
			void SetIndexBuffer( uint32_t* indexData, uint32_t size );
			// Material Handle
			void SetMaterial(ShaderMaterial material);
			ShaderMaterial GetMaterial();
			void Render();
			void UnRender();
			void AllocGpu();
			void FreeGpu();
	};
}; // namespace Soon
