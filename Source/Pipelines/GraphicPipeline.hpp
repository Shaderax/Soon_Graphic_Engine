#pragma once

#include "Pipelines/BasePipeline.hpp"

#include <vector>
#include <list>
#include <unordered_map>

#include "Pipelines/GraphicPipelineConf.hpp"

#include "Vertex.hpp"
#include "MeshVertex.hpp"

namespace Soon
{
	/*
	 * TODO: In material ptr or raw data ?
	*/

	struct IdRender
	{
		uint32_t matId;
		uint32_t* inputId;
		uint32_t numInstance;
		bool	cached;
	};

	class GraphicPipeline : public BasePipeline
	{
	private:
		//std::vector<VkVertexInputBindingDescription> m_BindingDescription;
		std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;

		std::vector<IdRender> m_RenderData;
		GraphicPipelineConf* graphicConf;

		MeshVertexDescription	m_MeshVertexInput;
		std::list<InputBindingDescription>	m_VertexInput;
	public:
		GraphicPipeline(GraphicPipelineConf* conf);
		~GraphicPipeline();
		void Init( void );
		// TODO: DESTROY _mUbm.CreateDescriptorSetLayout()
		void RecreatePipeline(void);
		//void GetBindingDescription(void);
		std::vector<VkVertexInputBindingDescription> GetBindingDescription( void );
		VertexDescription GetAdditionalVertexInput();
		MeshVertexDescription GetMeshVertexDescription();
		void BindCaller(VkCommandBuffer commandBuffer, uint32_t currentImage);
		
		void Render(uint32_t id);
		void UnRender(uint32_t id);
		uint32_t CreateNewId( void );
		void RemoveId(uint32_t id);
		void SetMeshId(uint32_t matId, uint32_t meshId);
		bool IsValidToRender(uint32_t id) const;
		void SetNumInstance(uint32_t id, uint32_t instances);

		void GetInputBindings( spv_reflect::ShaderModule& reflection );
		void SetAttributeDescriptionOffset( uint32_t binding, VertexDescription description );
		void SetBindingVertexInput( uint32_t idMat, uint32_t binding, const GpuBuffer& buffer);
	};
} // namespace Soon