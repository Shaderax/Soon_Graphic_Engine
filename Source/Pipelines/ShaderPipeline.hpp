#pragma once

#include "Pipelines/BasePipeline.hpp"

#include <vector>
#include <unordered_map>

namespace Soon
{
	/*
	 * In material ptr or raw data ?
	*/

	enum class ECullMode : uint8_t
	{
		NONE = 0,
    	FRONT = 1,
    	BACK = 2,
    	FRONT_AND_BACK = 3
	};

	VkCullModeFlagBits CullModeToVk(ECullMode cull);

	struct ShaderProperty
	{
		void* data = nullptr;
		uint32_t size = 0;
	};

	class ShaderPipeline : public BasePipeline
	{
	private:
		std::unordered_map<std::string, ShaderProperty>	m_Properties;
		using PropertiesIterator = std::unordered_map<std::string, ShaderProperty>::iterator;
	public:
		std::string _pathVert;
		std::string _pathFrag;
		static const PipelineType _type = PipelineType::GRAPHIC;

		void SetShaderProperties(std::string name, void* data);
		ShaderPipeline();
		~ShaderPipeline();
		void Init( void );
		// TODO: DESTROY _mUbm.CreateDescriptorSetLayout()
		void RecreatePipeline(void);
	};
} // namespace Soon
