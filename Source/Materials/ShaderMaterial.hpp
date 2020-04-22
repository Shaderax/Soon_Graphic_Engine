#pragma once

#include "Core/Renderer/Pipelines/ShaderPipeline.hpp"
#include "Core/Scene/Materials/Material.hpp"

#include "Core/Renderer/Vulkan/GraphicsRenderer.hpp"

namespace Soon
{
	class ShaderMaterial : public Material
	{
		public:
			uint32_t			_id;
			ShaderPipeline*		_shaderPipeline;

			ShaderMaterial( void );
			~ShaderMaterial( void );

			void Render( Transform3D& tr, Mesh* mesh );
			void Unrender( uint32_t id );

			template<class T>
				void SetPipeline( void )
				{
					if (_shaderPipeline)
						_shaderPipeline->RemoveFromPipeline(_id);
					_shaderPipeline = GraphicsRenderer::GetInstance()->AddPipeline<T>();
				}
	};
}
