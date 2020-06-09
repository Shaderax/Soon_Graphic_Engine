#pragma once

#include "Pipelines/PipelineJson.hpp"
#include <unordered_map>

namespace Soon
{
    enum class EPipelineType : uint8_t
	{
		GRAPHIC = 0,
		COMPUTE = 1
	};

	struct ShaderProperty
	{
		void* data = nullptr;
		uint32_t size = 0;
	};

	class PipelineConf
	{
	private:
        EPipelineType m_PipelineType;
		std::unordered_map<std::string, ShaderProperty>	m_Properties;
		using PropertiesIterator = std::unordered_map<std::string, ShaderProperty>::iterator;
	public:
	}
} // namespace Soon
