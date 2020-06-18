#include "Materials/GraphicMaterial.hpp"

#include "Utilities/Error.hpp"

namespace Soon
{
	GraphicMaterial::GraphicMaterial(void)
	{
		_id = TypeIdError;
		_shaderPipeline = nullptr;
		SetPipeline("./Ressources/Shaders/TestShader/NewDefaultPipeline.json");
	}

	GraphicMaterial::~GraphicMaterial(void)
	{
		if (_shaderPipeline && _id != IdError)
			_shaderPipeline->RemoveId(_id);
		_shaderPipeline = nullptr;
	}

	void GraphicMaterial::Render(std::uint32_t meshId)
	{
		if (_shaderPipeline && _id != Soon::IdError)
			_shaderPipeline->Render(_id);
		else if (_shaderPipeline && _id == Soon::IdError)
			_id = _shaderPipeline->CreateNewId();
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

	void GraphicMaterial::SetMesh(std::uint32_t meshId)
	{
		if (_shaderPipeline)
			_shaderPipeline->SetMeshId(_id, meshId);
	}
} // namespace Soon
