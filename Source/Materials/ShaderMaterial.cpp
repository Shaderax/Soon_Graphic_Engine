#include "Materials/ShaderMaterial.hpp"

#include "Pipelines/NewDefaultPipeline.hpp"

#include "Utilities/Error.hpp"

namespace Soon
{
	ShaderMaterial::ShaderMaterial( void )
	{
		_id = TypeIdError;
		_shaderPipeline = nullptr;
		SetPipeline<NewDefaultPipeline>();
	}

	ShaderMaterial::~ShaderMaterial( void )
	{
		if (_shaderPipeline && _id != IdError)
			_shaderPipeline->RemoveFromPipeline(_id);
		_shaderPipeline = nullptr;
	}
}
