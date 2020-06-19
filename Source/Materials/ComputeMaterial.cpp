#include "Materials/ComputeMaterial.hpp"

//#include "Pipelines/NewDefaultPipeline.hpp"

#include "Utilities/Error.hpp"

namespace Soon
{
	ComputeMaterial::ComputeMaterial(void)
	{
		_id = TypeIdError;
		m_Pipeline = nullptr;
		//SetPipeline<NewDefaultPipeline>();
		SetPipeline("./Ressources/Shaders/TestShader/NewDefaultPipeline.json");
	}

	ComputeMaterial::~ComputeMaterial(void)
	{
		if (m_Pipeline && _id != IdError)
			m_Pipeline->RemoveId(_id);
		m_Pipeline = nullptr;
	}
	void ComputeMaterial::Process()
	{
		if (m_Pipeline && _id != Soon::IdError)
			COMPUTEPIPELINE->Process(_id);
		else if (m_Pipeline && _id == Soon::IdError)
			_id = m_Pipeline->CreateNewId();
	}

	void ComputeMaterial::UnProcess(void)
	{
		if (m_Pipeline && _id != Soon::IdError)
			COMPUTEPIPELINE->UnProcess(_id);
	}

	void ComputeMaterial::RemoveFromPipeline(std::uint32_t meshId)
	{
		//if (_shaderPipeline)
		//	_id = _shaderPipeline->RemoveFromPipeline();
	}

	void ComputeMaterial::SetWorkGroup(uint32_t x, uint32_t y, uint32_t z)
	{
		if (m_Pipeline && _id != Soon::IdError)
			COMPUTEPIPELINE->SetWorkGroup(_id, x, y, z);
	}

} // namespace Soon
