#pragma once

#include "Pipelines/PipelineJson.hpp"

namespace Soon
{
	struct ShaderProperty
	{
		void* data = nullptr;
		uint32_t size = 0;
	};

	class GraphicPipelineConf
	{
	private:
		std::unordered_map<std::string, ShaderProperty>	m_Properties;
		using PropertiesIterator = std::unordered_map<std::string, ShaderProperty>::iterator;
	public:
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		VkViewport viewport = {};
		VkRect2D scissor = {};
		VkPipelineViewportStateCreateInfo viewportState = {};
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		VkPipelineMultisampleStateCreateInfo multisampling = {};
		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		VkGraphicsPipelineCreateInfo pipelineInfo = {};

		GraphicPipelineConf(void);
	};
} // namespace Soon
