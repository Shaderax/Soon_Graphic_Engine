#pragma once

#include "Pipelines/PipelineConf.hpp"
#include "vulkan/vulkan.h"

namespace Soon
{
	class ComputePipelineConf : public PipelineConf
	{
	public:
		ComputePipelineConf(void);
		VkComputePipelineCreateInfo pipelineInfo = {};
	};
} // namespace Soon