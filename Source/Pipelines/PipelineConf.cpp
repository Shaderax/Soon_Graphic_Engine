#include "Pipelines/PipelineConf.hpp"

#include <cstring>

namespace Soon
{
	std::unordered_map<std::string, PipelineProperty> Properties = {};

	PipelineConf::PipelineConf( EPipelineType type ) : m_PipelineType(type)
	{

	}

	void PipelineConf::AddUniqueSet(uint32_t set)
	{
		m_UniqueSets.push_back(set);
	}

	void PipelineConf::AddStageInfo( PipelineStage stage)
	{
		m_Stages.push_back(stage);
	}

	bool PipelineConf::IsUniqueSet( uint32_t set )
	{
		std::vector<uint32_t>::iterator it = std::find(m_UniqueSets.begin(), m_UniqueSets.end(), set);

		if (it != m_UniqueSets.end())
			return true;
		return false;
	}

	void PipelineConf::SetProperty(std::string name, std::string value)
	{
		if (m_Properties.find(name) == m_Properties.end())
			return ; // TODO: Not FOUND
		
		GenericProperty* prop = m_Properties[name].funct(value);
		memcpy(m_Properties[name].data, prop->GetData(), m_Properties[name].size);
		delete prop;
	}

	VkShaderStageFlagBits PipelineStageToVk( EPipelineStage stage )
	{
		switch (stage)
		{
			case EPipelineStage::VERTEX:
				return VK_SHADER_STAGE_VERTEX_BIT;
			case EPipelineStage::FRAGMENT:
				return VK_SHADER_STAGE_FRAGMENT_BIT;
			case EPipelineStage::TESSELLATION:
				return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			case EPipelineStage::COMPUTE:
				return VK_SHADER_STAGE_COMPUTE_BIT;
			case EPipelineStage::GEOMETRY:
				return VK_SHADER_STAGE_GEOMETRY_BIT;
			default :
				break;
		};
		return VK_SHADER_STAGE_VERTEX_BIT;
	}

	std::string PipelineConf::GetJsonPath( void ) const
	{
		return mJsonPath;
	}

	void PipelineConf::SetJsonPath( std::string name )
	{
		mJsonPath = name;
	}

} // namespace Soon
