#pragma once

#include "Pipelines/ShaderPipeline.hpp"
#include "Materials/Material.hpp"

#include "GraphicsRenderer.hpp"

namespace Soon
{
	class ShaderMaterial : public Material
	{
	public:
		ShaderPipeline* _shaderPipeline;

		ShaderMaterial(void);
		~ShaderMaterial(void);

		template <class T>
		void SetPipeline(void)
		{
			if (_shaderPipeline)
				_shaderPipeline->RemoveFromPipeline(_id);
			_shaderPipeline = GraphicsRenderer::GetInstance()->AddPipeline<T>();
		}

/*
		void SetMesh(std::uint32_t meshId)
		{
			if (_shaderPipeline)
				_id = _shaderPipeline->SetMesh(_id, meshId);
		}
*/

		void Render( std::uint32_t meshId );
		void UnRender( void );
		void RemoveFromPipeline(std::uint32_t meshId);
		bool HasValidVertexDescription(VertexDescription meshVD);
		void SetUniform( std::string name, void* data );
	};
} // namespace Soon
