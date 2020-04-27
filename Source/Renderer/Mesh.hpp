#pragma once

#include "Utilities/Vertex.hpp"
#include <stdint.h>
#include <limits>

#include <Materials/ShaderMaterial.hpp>

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
			Mesh(VertexDescription v);
			Mesh(const Mesh &other);
			~Mesh(void);
			// Loader Free ?

			void SetVertexElement(uint8_t* data, uint32_t size, VertexElement elem);
			// Material Handle
			void SetMaterial(ShaderMaterial material);
			ShaderMaterial GetMaterial();
			void Render();
			void UnRender();
			std::uint32_t AllocGpu();
			void FreeGpu();
	};
}; // namespace Soon
