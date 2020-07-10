#pragma once

#include "MeshVertex.hpp"
#include <stdint.h>
#include <limits>
#include <memory>

#include "Materials/GraphicMaterial.hpp"

#include "Utilities/Error.hpp"

namespace Soon
{
	class Mesh : RendererRessource
	{
		private:
			GraphicMaterial		m_Material;
			MeshVertexDescription	m_VertexDescription;

		public:
			//uint8_t*	mVertexData = nullptr;
			std::shared_ptr<uint8_t> mVertexData;
			std::shared_ptr<uint32_t>	mIndices;
			uint32_t	mNumIndices;
			uint32_t	mNumVertex;
			uint32_t	mVertexTotalSize = 0;

			Mesh( MeshVertexDescription v );
			Mesh( const Mesh &other );
			Mesh( const Mesh &&other );
			~Mesh( void );
			// Loader Free ?

			void	SetVertexElement( uint8_t* data, uint32_t size, MeshVertexElement elem );
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
