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

	void* ShaderMaterial::GetUniform(std::string name)
	{
		if (_shaderPipeline && _id != IdError)
			_shaderPipeline->Get(name, data, _id);

	}

	void ShaderMaterial::SetTexture(std::string name, Texture& texture)
	{
		if (_shaderPipeline && _id != IdError)
		{
			GraphicsRenderer::GetInstance()->AddTexture(&texture);
			std::cout << texture.GetId() << std::endl;
			_shaderPipeline->SetTexture(name, _id, texture.GetId());
		}
	}
	
	const ShaderPipeline* ShaderMaterial::GetPipeline( void ) const
	{
		return (_shaderPipeline);
	}

	void ShaderMaterial::SetPipeline(std::string name);
	{
		if (_shaderPipeline)
			_shaderPipeline->RemoveFromPipeline(_id);
		_shaderPipeline = GraphicsRenderer::GetInstance()->AddPipeline<T>();
	}
} // namespace Soon
