#pragma once

#include "ComputePipelineConf.hpp"

namespace Soon
{
	ComputePipelineConf::ComputePipelineConf(void) : PipelineConf(EPipelineType::COMPUTE)
	{
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	}
} // namespace Soon
