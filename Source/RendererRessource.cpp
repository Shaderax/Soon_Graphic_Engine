#include "RendererRessource.hpp"

#include "GraphicsRenderer.hpp"

#include "Mesh.hpp"
#include "Texture.hpp"

namespace Soon
{
	RendererRessource::RendererRessource(ERendererRessource type, uint32_t id) : m_UniqueId(id)
	{
		m_Type = type;

		if (id != Soon::IdError)
		{
			switch (m_Type)
			{
			case ERendererRessource::BUFFER:
				GraphicsRenderer::GetInstance()->AddBuffer(m_UniqueId);
				break ;
			case ERendererRessource::MESH:
				GraphicsRenderer::GetInstance()->AddMesh(m_UniqueId);
				break ;
			case ERendererRessource::TEXTURE:
				GraphicsRenderer::GetInstance()->AddTexture(m_UniqueId);
				break ;
			}
		}
		GraphicsRenderer::GetInstance()->AddRessource(m_Unique_Id);
	}

	RendererRessource::~RendererRessource(void)
	{
		if (m_UniqueId != Soon::IdError)
		{
			switch (m_Type)
			{
			case ERendererRessource::BUFFER:
				GraphicsRenderer::GetInstance()->RemoveBuffer(m_UniqueId);
				break ;
			case ERendererRessource::MESH:
				GraphicsRenderer::GetInstance()->RemoveMesh(m_UniqueId);
				break ;
			case ERendererRessource::TEXTURE:
				GraphicsRenderer::GetInstance()->RemoveTexture(m_UniqueId);
				break ;
			}
		}
	}

	uint32_t RendererRessource::GetId( void ) const
	{
		return (m_UniqueId);
	}
} // namespace Soon