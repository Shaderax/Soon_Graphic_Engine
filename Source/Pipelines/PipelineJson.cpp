#pragma once

#include <stdint.h>
#include <iostream>
#include <fstream>
#include "PipelineJson.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "vulkan/vulkan.h"

#include "Pipelines/GraphicPipelineConf.hpp"
#include "Pipelines/ComputePipelineConf.hpp"

#include <unordered_map>

namespace Soon
{
	std::unordered_map<std::string, EPipelineStage> ValidShaderPath =
	{
		{"Vertex", EPipelineStage::VERTEX},
		{"Fragment", EPipelineStage::FRAGMENT},
	};

	bool IsValidShader( std::string name )
	{
		return ValidShaderPath.find(name) != ValidShaderPath.end();
	}

	void ParseJson(json& j, PipelineConf& conf)
	{
		for (json::iterator it = j.begin(); it != j.end(); ++it)
		{
			if (it.key() == "ShaderPaths")
			{
				for (json::iterator shaderP = it->begin(); shaderP != it->end(); ++shaderP)
				{
					if (IsValidShader(shaderP.key()))
						conf.AddStageInfo({ValidShaderPath[shaderP.key()], shaderP.value()});
				}
			}
			else if (it.key() == "UniqueSets")
			{
				for (json::iterator shaderP = it->begin(); shaderP != it->end(); ++shaderP)
					conf.AddUniqueSet(shaderP.value());
			}
			else if (it.key() == "Properties")
			{
				for (json::iterator shaderP = it->begin(); shaderP != it->end(); ++shaderP)
					conf.SetProperty(shaderP.key(), shaderP.value());
			}
			std::cout << *it << '\n';
		}
	}

	PipelineConf* ReadPipelineJson( std::string path )
	{
		std::ifstream i(path);

		if (!i.is_open())
			std::runtime_error("Can't open Pipeline Json : " + path + "\n");

		json j;
		i >> j;

		if (!j[0].contains("Type"))
			std::runtime_error("Undefined Type in Pipeline Json : " + path + "\n");
		if (!j[0].contains("ShaderPaths"))
			std::runtime_error("Undefined ShaderPaths in Pipeline Json : " + path + "\n");

		PipelineConf* conf = nullptr;

		if (j[0]["Type"] == "Graphic")
			conf = new GraphicPipelineConf();
		else if (j[0]["Type"] == "Compute")
			conf = new ComputePipelineConf();

		ParseJson(j[0], *conf);

		return conf;
	}
}