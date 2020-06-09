#pragma once

#include "Pipelines/BasePipeline.hpp"

#include <vector>
#include <unordered_map>

namespace Soon
{
	/*
	 * TODO: In material ptr or raw data ?
	*/

	struct IdRender
	{
		uint32_t matId;
		uint32_t meshId;
		bool	cached;
	};

	class GraphicPipeline : public BasePipeline
	{
	private:
		VertexDescription _vertexDescription;
		std::vector<IdRender> m_RenderData;
		VkPipeline _graphicPipeline;
		GraphicsPipelineConf _conf;
	public:
		std::string _pathVert;
		std::string _pathFrag;
		const PipelineType _type = PipelineType::GRAPHIC;

		void SetPipelineProperties(std::string name, void* data);
		GraphicPipeline();
		~GraphicPipeline();
		void Init( void );
		// TODO: DESTROY _mUbm.CreateDescriptorSetLayout()
		void RecreatePipeline(void);
	};
} // namespace Soon
