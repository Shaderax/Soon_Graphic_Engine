#pragma once

#include "Vertex.hpp"
#include <stdint.h>
#include <limits>

#include "Materials/GraphicMaterial.hpp"

#include "Utilities/Error.hpp"

namespace Soon
{
	class Mesh
	{
		private:
			GraphicMaterial		m_Material;
			VertexDescription	m_VertexDescription;

			uint32_t	m_UniqueId = Soon::IdError;

		public:
			uint8_t*	mVertexData = nullptr;
			uint32_t*	mIndices;
			uint32_t	mNumIndices;
			uint32_t	mNumVertex;
			uint32_t	mVertexTotalSize = 0;

			Mesh( VertexDescription v );
			Mesh( const Mesh &other );
			Mesh( const Mesh &&other );
			~Mesh( void );
			// Loader Free ?

			void	SetVertexElement( uint8_t* data, uint32_t size, VertexElement elem );
			void	SetIndexBuffer( uint32_t* indexData, uint32_t size );
			// Material Handle
			void	SetMaterial( GraphicMaterial& material );
			GraphicMaterial&	GetMaterial( void );
			void	Render( void );
			void	UnRender( void );
			void	AllocGpu( void );
			void	FreeGpu( void );
	};
} // namespace Soon
