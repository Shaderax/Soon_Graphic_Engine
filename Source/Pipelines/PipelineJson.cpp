#include <stdint.h>
#include <iostream>
#include <fstream>
#include "PipelineJson.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "vulkan/vulkan.h"

#include "Pipelines/PipelineConf.hpp"
#include "Pipelines/GraphicPipelineConf.hpp"
#include "Pipelines/ComputePipelineConf.hpp"

#include <unordered_map>

namespace Soon
{
	std::unordered_map<std::string, EPipelineStage> ValidShaderPath =
	{
		{"Vertex", EPipelineStage::VERTEX},
		{"Fragment", EPipelineStage::FRAGMENT},
		{"Compute", EPipelineStage::COMPUTE}
	};

	bool IsValidShader( std::string name )
	{
		return ValidShaderPath.find(name) != ValidShaderPath.end();
	}

	void ParseGraphicJson(json& j, GraphicPipelineConf* conf)
	{
		for (json::iterator it = j.begin(); it != j.end(); ++it)
		{
			if (it.key() == "InputBindings")
			{
				for (json::iterator shaderP = it->begin(); shaderP != it->end(); ++shaderP)
				{
					conf->AddInputBindings(shaderP.key(), shaderP.value());
				}
			}
			else if (it.key() == "InstanceBinding")
			{
				for (json::iterator shaderP = it->begin(); shaderP != it->end(); ++shaderP)
				{
					// TODO: HERE
					conf->SetBindingInputRate(shaderP.value(), VK_VERTEX_INPUT_RATE_INSTANCE);
					//HAVE TO SAVE IN VEC_OF_INSTZNCE or by input binding
				}
			}
		}
	}

	void ParseComputeJson(json& j, ComputePipelineConf* conf)
	{

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
				{
					conf.SetProperty(shaderP.key(), shaderP.value());
				}
			}
		}
	}

	PipelineConf* ReadPipelineJson( std::string path )
	{
		std::ifstream i(path);

		if (!i.is_open())
			throw std::runtime_error("Can't open Pipeline Json : " + path + "\n");

		json j = json::parse(i);

		if (!j[0].contains("Type"))
			throw std::runtime_error("Undefined Type in Pipeline Json : " + path + "\n");
		if (!j[0].contains("ShaderPaths"))
			throw std::runtime_error("Undefined ShaderPaths in Pipeline Json : " + path + "\n");

		PipelineConf* conf = nullptr;

		if (j[0]["Type"] == "Graphic")
		{
			conf = new GraphicPipelineConf();
			ParseGraphicJson(j[0], static_cast<GraphicPipelineConf*>(conf));
		}
		else if (j[0]["Type"] == "Compute")
		{
			conf = new ComputePipelineConf();
			ParseComputeJson(j[0], static_cast<ComputePipelineConf*>(conf));
		}

		ParseJson(j[0], *conf);

		conf->SetJsonPath(path);

		if (conf->GetStages().size() == 0)
			throw std::runtime_error("No Valid Stage Found in Json");

		return conf;
	}
}