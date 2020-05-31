#pragma once

#include "Pipelines/BasePipeline.hpp"

#include <vector>
#include <unordered_map>

namespace Soon
{
	/*
	 * TODO: In material ptr or raw data ?
	*/

	class GraphicPipeline : public BasePipeline
	{
	public:
		std::string _pathVert;
		std::string _pathFrag;
		const PipelineType _type = PipelineType::GRAPHIC;

		void SetShaderProperties(std::string name, void* data);
		GraphicPipeline();
		~GraphicPipeline();
		void Init( void );
		// TODO: DESTROY _mUbm.CreateDescriptorSetLayout()
		void RecreatePipeline(void);
	};
} // namespace Soon
