#pragma once

#include "Renderer/Pipelines/ShaderPipeline.hpp"
#include "Materials/Material.hpp"

#include "Renderer/Vulkan/GraphicsRenderer.hpp"

namespace Soon
{
	class ShaderMaterial : public Material
	{
	public:
		ShaderPipeline *_shaderPipeline;

		ShaderMaterial(void);
		~ShaderMaterial(void);

		template <class T>
		void SetPipeline(void)
		{
			if (_shaderPipeline)
				_shaderPipeline->RemoveFromPipeline(_id);
			_shaderPipeline = GraphicsRenderer::GetInstance()->AddPipeline<T>();
		}

		void SetMesh(std::uint32_t meshId)
		{
			//if (_shaderPipeline)
			//	_id = _shaderPipeline->SetMesh(_id, meshId);
		}

		void AddToPipeline(std::uint32_t meshId)
		{
			if (_shaderPipeline)
				_id = _shaderPipeline->AddToPipeline(meshId);
		}

		void RemoveFromPipeline(std::uint32_t meshId)
		{
			//if (_shaderPipeline)
			//	_id = _shaderPipeline->RemoveFromPipeline();
		}

		bool HasValidVertexDescription(VertexDescription meshVD)
		{
			if (_shaderPipeline == nullptr)
				return (false);
			return (meshVD == _shaderPipeline->GetVertexDescription());
		}

		 void SetUniform( std::string name, void* data )
		 {
			 if (_shaderPipeline && _id != IdError)
			 	_shaderPipeline->Set(name, data, _id);
		 }
	};
} // namespace Soon
