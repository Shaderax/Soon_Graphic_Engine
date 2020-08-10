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
		/*
		Donc on peut avoir une mesh pour plusieurs mat ?
			Ui 
		Donc on peut avoir un mat pour plusieurs mesh ?
			Genre a quoi ca sert ? juste ca sera au meme endoirt mais ca sert a rien
		Donc qui gere la quand le graphic material est a détruire ?


		Do i tej the copy construct ?
		je peux avoir 2 mesh ui mais pas 2 mat ?
		uint32_t 		_id = Soon::IdError;
		BasePipeline*	m_Pipeline = nullptr;

		*/
	}

	GraphicMaterial::GraphicMaterial(const GraphicMaterial& other)
	{
		_id = other._id;
		m_Pipeline = other.m_Pipeline;
	}

	GraphicMaterial::GraphicMaterial(GraphicMaterial&& other)
	{
		_id = std::exchange(other._id, Soon::IdError);
		m_Pipeline = std::exchange(other.m_Pipeline, nullptr);
	}

	GraphicMaterial& GraphicMaterial::operator=(const GraphicMaterial& other)
	{
		if (this == &other)
			return *this;

		_id = other._id;
		m_Pipeline = other.m_Pipeline;

		return *this;
	}

	GraphicMaterial& GraphicMaterial::operator=(GraphicMaterial&& other)
	{
		if (this == &other)
			return *this;

		_id = std::exchange(other._id, Soon::IdError);
		m_Pipeline = std::exchange(other.m_Pipeline, nullptr);

		return *this;
	}

	void GraphicMaterial::Render(std::uint32_t meshId)
	{
		if (m_Pipeline && _id == Soon::IdError)
		{
			_id = GRAPHICPIPELINE->CreateNewId();
			GRAPHICPIPELINE->SetMeshId(_id, meshId);
			GRAPHICPIPELINE->Render(_id);
		}
		else if (m_Pipeline && _id != Soon::IdError)
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

	bool GraphicMaterial::HasValidVertexDescription(MeshVertexDescription meshVD)
	{
		if (m_Pipeline == nullptr)
			return (false);
		return (meshVD == GRAPHICPIPELINE->GetMeshVertexDescription());
	}

	void GraphicMaterial::SetMesh(std::uint32_t meshId)
	{
		if (m_Pipeline && _id != Soon::IdError)
			GRAPHICPIPELINE->SetMeshId(_id, meshId);
	}
} // namespace Soon
