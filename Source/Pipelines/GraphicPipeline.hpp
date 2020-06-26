#pragma once

#include "Pipelines/BasePipeline.hpp"

#include <vector>
#include <unordered_map>

#include "Pipelines/GraphicPipelineConf.hpp"

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
		GraphicPipelineConf* graphicConf;
	public:
		GraphicPipeline(GraphicPipelineConf* conf);
		~GraphicPipeline();
		void Init( void );
		// TODO: DESTROY _mUbm.CreateDescriptorSetLayout()
		void RecreatePipeline(void);
		void GetBindingDescription(void);
		VertexDescription GetVertexDescription();
		void BindCaller(VkCommandBuffer commandBuffer, uint32_t currentImage);
		
		void Render(uint32_t id);
		void UnRender(uint32_t id);
		uint32_t CreateNewId( void );
		void RemoveId(uint32_t id);

		void SetMeshId(uint32_t matId, uint32_t meshId);

		void GetInputBindings( spv_reflect::ShaderModule& reflection );

	};
} // namespace Soon