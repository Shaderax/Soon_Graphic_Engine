#pragma once

#include <stdint.h>
#include "PipelineConf.hpp"

namespace Soon
{
	enum class ECullMode : uint8_t
	{
		NONE = 0,
    	FRONT = 1,
    	BACK = 2,
    	FRONT_AND_BACK = 3
	};

	VkCullModeFlagBits CullModeToVk(ECullMode cull);

	enum class EFrontFace : uint8_t
	{
		COUNTER_CLOCKWISE = 0,
    	CLOCKWISE = 1,
	};

	GraphicPipelineConf ReadPipelineJson( void );
}