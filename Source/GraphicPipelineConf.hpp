#pragma once

#include "Pipelines/PipelineJson.hpp"

namespace Soon
{
	class GraphicPipelineConf : PipelineConf
	{
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
