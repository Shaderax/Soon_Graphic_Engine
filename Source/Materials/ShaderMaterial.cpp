#include "Core/Pipelines/DefaultPipeline.hpp"
#include "Core/Scene/Materials/ShaderMaterial.hpp"

namespace Soon
{
	ShaderMaterial::ShaderMaterial( void )
	{
		_id = TypeIdError;
		_shaderPipeline = nullptr;
		SetPipeline<DefaultPipeline>();
	}

	ShaderMaterial::~ShaderMaterial( void )
	{
		if (_shaderPipeline && _id != TypeIdError)
			_shaderPipeline->RemoveFromPipeline(_id);
		_shaderPipeline = nullptr;
	}

	void ShaderMaterial::Render( Transform3D& tr, Mesh* mesh )
	{
		if (_shaderPipeline && _id != TypeIdError)
			_shaderPipeline->Render(_id);
		else if (_shaderPipeline)
			_id = _shaderPipeline->AddToPipeline(tr, mesh);
	}

	void ShaderMaterial::Unrender( uint32_t id )
	{
		if (_shaderPipeline && _id != TypeIdError)
			_shaderPipeline->UnRender(_id);
		_id = TypeIdError;
	}
}
