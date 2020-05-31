#pragma once

#include <stdint.h>
#include <iostream>
#include <fstream>
#include "PipelineJson.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "vulkan/vulkan.h"

namespace Soon
{
	VkCullModeFlagBits CullModeToVk(ECullMode cull)
	{
		switch (cull)
		{
		case ECullMode::NONE:
			return VK_CULL_MODE_NONE;
		case ECullMode::BACK:
			return VK_CULL_MODE_BACK_BIT;
		case ECullMode::FRONT:
			return VK_CULL_MODE_FRONT_BIT;
		case ECullMode::FRONT_AND_BACK:
			return VK_CULL_MODE_FRONT_AND_BACK;
		default:
			return VK_CULL_MODE_NONE;
		}
		return VK_CULL_MODE_NONE;
	}

	GraphicPipelineConf ReadPipelineJson( std::string path )
	{
		std::ifstream i(path);

		if (!i.is_open())
			std::runtime_error("Can't open Pipeline Json : " << path << std::endl;

		json j;
		i >> j;

		if (!j.contains("Type"))
			std::runtime_error("Undefined Type in Pipeline Json : " << path << std::endl;
		if (!j.contains("ShaderPaths"))
			std::runtime_error("Undefined ShaderPaths in Pipeline Json : " << path << std::endl;
	}
}