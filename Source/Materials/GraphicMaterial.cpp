#include "Materials/GraphicMaterial.hpp"

#include "Utilities/Error.hpp"

namespace Soon
{
	GraphicMaterial::GraphicMaterial(void)
	{
		_id = Soon::IdError;
		m_Pipeline = nullptr;
		SetPipeline("./Examples/NewDefaultPipeline.json");
	}

	GraphicMaterial::~GraphicMaterial(void)
	{
		if (m_Pipeline && _id != IdError)
			GRAPHICPIPELINE->RemoveId(_id);
		m_Pipeline = nullptr;
	}

	void GraphicMaterial::Render(std::uint32_t meshId)
	{
		if (m_Pipeline && _id == Soon::IdError)
			_id = GRAPHICPIPELINE->CreateNewId();
		if (m_Pipeline && _id != Soon::IdError)
		{
			GRAPHICPIPELINE->SetMeshId(_id, meshId);
			GRAPHICPIPELINE->Render(_id);
		}
	}

	void GraphicMaterial::UnRender(void)
	{
		if (m_Pipeline && _id != Soon::IdError)
			GRAPHICPIPELINE->UnRender(_id);
	}

	void GraphicMaterial::RemoveFromPipeline(std::uint32_t meshId)
	{
		//if (GRAPHICPIPELINE)
		//	_id = GRAPHICPIPELINE->RemoveFromPipeline();
	}

	bool GraphicMaterial::HasValidVertexDescription(VertexDescription meshVD)
	{
		if (m_Pipeline == nullptr)
			return (false);
		return (meshVD == GRAPHICPIPELINE->GetVertexDescription());
	}

	void GraphicMaterial::SetMesh(std::uint32_t meshId)
	{
		if (m_Pipeline && _id != Soon::IdError)
			GRAPHICPIPELINE->SetMeshId(_id, meshId);
	}
} // namespace Soon
