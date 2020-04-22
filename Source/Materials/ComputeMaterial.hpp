#pragma once

#include "Core/Renderer/Pipelines/ComputePipeline.hpp"
#include "Core/Scene/Materials/Material.hpp"

#include "Core/Renderer/Vulkan/GraphicsRenderer.hpp"

namespace Soon
{
	class ComputeMaterial : public Material
	{
		public:
			uint32_t		_id;
			ComputePipeline*	_computePipeline;

			template<class T>
				void SetPipeline( void )
				{
					if (_computePipeline)
						_computePipeline->RemoveFromPipeline(_id);
					_computePipeline = GraphicsRenderer::GetInstance()->AddPipeline<T>();
				}
	};
}
