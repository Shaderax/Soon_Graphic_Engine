#pragma once

#include "Pipelines/GraphicPipeline.hpp"
#include "Materials/Material.hpp"

#include "GraphicsRenderer.hpp"

namespace Soon
{
	class GraphicMaterial : public Material
	{
	private:
		ShaderPipeline* _shaderPipeline;
	public:

		GraphicMaterial(void);
		~GraphicMaterial(void);

		const ShaderPipeline* GetPipeline( void ) const;

		void SetPipeline(std::string name);

		void SetMesh(std::uint32_t meshId);

		void Render( std::uint32_t meshId );
		void UnRender( void );
		void RemoveFromPipeline(std::uint32_t meshId);
		bool HasValidVertexDescription(VertexDescription meshVD);
		void SetUniform( std::string name, void* data );
		void* GetUniform( std::string name );
		void SetTexture(std::string name, Texture& texture);
	};
} // namespace Soon
