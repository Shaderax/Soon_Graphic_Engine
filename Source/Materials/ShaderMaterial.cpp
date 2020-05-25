#include "Materials/ShaderMaterial.hpp"

#include "Pipelines/NewDefaultPipeline.hpp"

#include "Utilities/Error.hpp"

namespace Soon
{
	ShaderMaterial::ShaderMaterial(void)
	{
		_id = TypeIdError;
		_shaderPipeline = nullptr;
		SetPipeline<NewDefaultPipeline>();
	}

	ShaderMaterial::~ShaderMaterial(void)
	{
		if (_shaderPipeline && _id != IdError)
			_shaderPipeline->RemoveFromPipeline(_id);
		_shaderPipeline = nullptr;
	}
	void ShaderMaterial::Render(std::uint32_t meshId)
	{
		if (_shaderPipeline && _id != Soon::IdError)
			_shaderPipeline->Render(_id);
		else if (_shaderPipeline && _id == Soon::IdError)
			_id = _shaderPipeline->AddToPipeline(meshId);
	}

	void ShaderMaterial::UnRender(void)
	{
		if (_shaderPipeline && _id != Soon::IdError)
			_shaderPipeline->UnRender(_id);
	}

	void ShaderMaterial::RemoveFromPipeline(std::uint32_t meshId)
	{
		//if (_shaderPipeline)
		//	_id = _shaderPipeline->RemoveFromPipeline();
	}

	bool ShaderMaterial::HasValidVertexDescription(VertexDescription meshVD)
	{
		if (_shaderPipeline == nullptr)
			return (false);
		return (meshVD == _shaderPipeline->GetVertexDescription());
	}

	void ShaderMaterial::SetUniform(std::string name, void *data)
	{
		if (_shaderPipeline && _id != IdError)
			_shaderPipeline->Set(name, data, _id);
	}
} // namespace Soon
