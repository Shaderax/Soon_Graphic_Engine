#pragma once

#include "Pipelines/GraphicPipeline.hpp"
#include "Materials/Material.hpp"

#include "GraphicsRenderer.hpp"

#define GRAPHICPIPELINE reinterpret_cast<GraphicPipeline*>(m_Pipeline)

namespace Soon
{
	class GraphicMaterial : public Material
	{
	private:
		//using _shaderPipeline = (GraphicPipeline*)m_Pipeline;
		//constexpr GraphicPipeline* _shaderPipeline = reinterpret_cast<GraphicPipeline*>(m_Pipeline);
	public:
		GraphicMaterial(void);
		~GraphicMaterial(void);

		void SetMesh(std::uint32_t meshId);

		void Render( std::uint32_t meshId );
		void UnRender( void );
		void RemoveFromPipeline(std::uint32_t meshId);
		bool HasValidVertexDescription(MeshVertexDescription meshVD);
	};
} // namespace Soon
