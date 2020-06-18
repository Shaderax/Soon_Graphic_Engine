#include "Materials/GraphicMaterial.hpp"

//#include "Pipelines/NewDefaultPipeline.hpp"

#include "Utilities/Error.hpp"

namespace Soon
{
	GraphicMaterial::GraphicMaterial(void)
	{
		_id = TypeIdError;
		_shaderPipeline = nullptr;
		//SetPipeline<NewDefaultPipeline>();
		SetPipeline("./Ressources/Shaders/TestShader/NewDefaultPipeline.json");
	}

	GraphicMaterial::~GraphicMaterial(void)
	{
		if (_shaderPipeline && _id != IdError)
			_shaderPipeline->RemoveFromPipeline(_id);
		_shaderPipeline = nullptr;
	}
	void GraphicMaterial::Render(std::uint32_t meshId)
	{
		if (_shaderPipeline && _id != Soon::IdError)
			_shaderPipeline->Render(_id);
		else if (_shaderPipeline && _id == Soon::IdError)
			_id = _shaderPipeline->AddToPipeline(meshId);
	}

	void GraphicMaterial::UnRender(void)
	{
		if (_shaderPipeline && _id != Soon::IdError)
			_shaderPipeline->UnRender(_id);
	}

	void GraphicMaterial::RemoveFromPipeline(std::uint32_t meshId)
	{
		//if (_shaderPipeline)
		//	_id = _shaderPipeline->RemoveFromPipeline();
	}

	bool GraphicMaterial::HasValidVertexDescription(VertexDescription meshVD)
	{
		if (_shaderPipeline == nullptr)
			return (false);
		return (meshVD == _shaderPipeline->GetVertexDescription());
	}

	void GraphicMaterial::SetUniform(std::string name, void *data)
	{
		if (_shaderPipeline && _id != IdError)
			_shaderPipeline->Set(name, data, _id);
	}

	void* GraphicMaterial::GetUniform(std::string name)
	{
		if (_shaderPipeline && _id != IdError)
			_shaderPipeline->Get(name, data, _id);

	}

	void GraphicMaterial::SetTexture(std::string name, Texture& texture)
	{
		if (_shaderPipeline && _id != IdError)
		{
			GraphicsRenderer::GetInstance()->AddTexture(&texture);
			std::cout << texture.GetId() << std::endl;
			_shaderPipeline->SetTexture(name, _id, texture.GetId());
		}
	}
	
	const GraphicPipeline* GraphicMaterial::GetPipeline( void ) const
	{
		return (_shaderPipeline);
	}

	void GraphicMaterial::SetPipeline(std::string name);
	{
		if (_shaderPipeline)
			_shaderPipeline->RemoveFromPipeline(_id);
		_shaderPipeline = GraphicsRenderer::GetInstance()->AddPipeline<T>();
	}

	void GraphicMaterial::SetMesh(std::uint32_t meshId)
	{
		if (_shaderPipeline)
			_shaderPipeline->SetMesh(_id, meshId);
	}

} // namespace Soon
