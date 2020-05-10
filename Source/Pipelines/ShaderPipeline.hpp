#pragma once

#include "Pipelines/BasePipeline.hpp"

#include <vector>

namespace Soon
{
	class Mesh;

	/*
	 * In material ptr or raw data ?
*/

	class ShaderPipeline : public BasePipeline
	{
	private:
	public:
		std::string _pathVert;
		std::string _pathFrag;
		static const PipelineType _type = PipelineType::GRAPHIC;

		ShaderPipeline()
		{
		}

		~ShaderPipeline()
		{
			//if (_graphicPipeline != nullptr)
			//	vkDestroyPipeline(GraphicsInstance::GetInstance()->GetDevice(), _graphicPipeline, nullptr);
		}

		void Init(void)
		{
			assert(!_pathVert.empty() && !_pathFrag.empty() && "Vertex Path or Frag Path not feed");

			GetShaderData(_pathVert);
			GetShaderData(_pathFrag);

			GetBindingDescription();

			_conf.vertexInputInfo.vertexBindingDescriptionCount = 1;
			_conf.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(_attributeDescriptions.size());
			_conf.vertexInputInfo.pVertexBindingDescriptions = &_bindingDescription;
			_conf.vertexInputInfo.pVertexAttributeDescriptions = _attributeDescriptions.data();

			_descriptorSetLayout = GraphicsInstance::GetInstance()->CreateDescriptorSetLayout(uboLayoutBinding);
			_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_descriptorSetLayout);
			_conf.pipelineInfo.layout = _pipelineLayout;
			_graphicPipeline = GraphicsInstance::GetInstance()->CreateGraphicsPipeline(
				_conf,
				_pathVert,
				_pathFrag);

			SetUniformsArray();

			// Alloc Camera
		}

		void RecreatePipeline(void)
		{
			VkExtent2D Extent = GraphicsInstance::GetInstance()->GetSwapChainExtent();
			_conf.viewport.width = (float)Extent.width;
			_conf.viewport.height = (float)Extent.height;

			_conf.scissor.extent = Extent;

			_conf.pipelineInfo.renderPass = GraphicsInstance::GetInstance()->GetRenderPass();
			_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_descriptorSetLayout);
			_conf.pipelineInfo.layout = _pipelineLayout;
			_graphicPipeline = GraphicsInstance::GetInstance()->CreateGraphicsPipeline(
				_conf,
				_pathVert,
				_pathFrag);
		}
	};
} // namespace Soon
