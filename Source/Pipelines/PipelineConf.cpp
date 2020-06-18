#pragma once

#include "Pipelines/PipelineConf.hpp"

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
} // namespace Soon
